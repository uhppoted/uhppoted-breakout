package auth

import (
	"crypto/hmac"
	"crypto/sha1"
	"encoding/base32"
	"encoding/binary"
	"fmt"
	"math"
	"slices"
	"testing"

	"github.com/pquerna/otp/hotp"
)

func TestNextID(t *testing.T) {
	expected := "848788"

	if code, err := hotp.GenerateCode("A3WYPV4VWQLL6VFZ", 1); err != nil {
		t.Fatalf("%v", err)
	} else if code != expected {
		t.Errorf("invalid code for counter=1 - expected:%v, got:%v", expected, code)
	}
}

// NTS debugging firmware HOTP implementation
func TestGenerateCodeCustom(t *testing.T) {
	secret := "A3WYPV4VWQLL6VFZ"

	key, _ := base32.StdEncoding.DecodeString(secret)
	if !slices.Equal(key, []uint8{0x06, 0xed, 0x87, 0xd7, 0x95, 0xb4, 0x16, 0xbf, 0x54, 0xb9}) {
		t.Errorf("incorrect key - expected:%v, got:%x", "06ed87d795b416bf54b9", key)
	}

	counter := uint64(1)
	buffer := make([]uint8, 8)
	binary.BigEndian.PutUint64(buffer, counter)
	if !slices.Equal(buffer, []uint8{0, 0, 0, 0, 0, 0, 0, 1}) {
		t.Errorf("incorrect counter bytes - expected:%v, got:%v", []uint8{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}, buffer)
	}

	MAC := hmac.New(sha1.New, key)

	MAC.Write(buffer)
	sum := MAC.Sum(nil)

	if !slices.Equal(sum, []uint8{0x3d, 0x90, 0x49, 0x06, 0x42, 0x1f, 0xff, 0x0e, 0xff, 0x82, 0xc5, 0x50, 0xef, 0x20, 0xd4, 0xf3, 0xfc, 0x21, 0xa4, 0x7b}) {
		t.Errorf("incorrect HMAC - expected:%v, got:%x", "3d904906421fff0eff82c550ef20d4f3fc21a47b", sum)
	}

	// RFC 4226 dynamic truncation (http://tools.ietf.org/html/rfc4226#section-5.4)
	offset := sum[len(sum)-1] & 0xf
	value := int64(((int(sum[offset]) & 0x7f) << 24) |
		((int(sum[offset+1] & 0xff)) << 16) |
		((int(sum[offset+2] & 0xff)) << 8) |
		(int(sum[offset+3]) & 0xff))

	if value != 1357848788 {
		t.Errorf("incorrectly truncated value - expected:%v, got:%v", 1357848788, value)
	}

	digits := 6
	mod := int32(value % int64(math.Pow10(digits)))

	if mod != 848788 {
		t.Errorf("incorrectly int32 value - expected:%v, got:%v", 848788, mod)
	}

	if v := fmt.Sprintf("%06v", mod); v != "848788" {
		t.Errorf("incorrectly HOTP - expected:%v, got:%v", "848788", v)
	}
}
