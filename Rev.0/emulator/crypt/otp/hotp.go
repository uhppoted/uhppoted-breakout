package otp

import (
	"crypto/hmac"
	"crypto/sha1"
	"encoding/base32"
	"encoding/binary"
	"fmt"
)

func GenerateHOTP(secret string, counter uint64) (string, error) {
	if key, err := base32.StdEncoding.DecodeString(secret); err != nil {
		return "", err
	} else {
		buffer := make([]uint8, 8)
		binary.BigEndian.PutUint64(buffer, counter)

		MAC := hmac.New(sha1.New, key)

		MAC.Write(buffer)
		hash := MAC.Sum(nil)

		// ... truncate to 6 digits (cf. http://tools.ietf.org/html/rfc4226#section-5.4)
		offset := hash[len(hash)-1] & 0xf
		value := int64(((int(hash[offset]) & 0x7f) << 24) |
			((int(hash[offset+1] & 0xff)) << 16) |
			((int(hash[offset+2] & 0xff)) << 8) |
			(int(hash[offset+3]) & 0xff))

		mod := int32(value % 1000000)

		return fmt.Sprintf("%06v", mod), nil
	}
}
