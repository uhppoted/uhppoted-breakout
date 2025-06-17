package main

import (
	"flag"
	"fmt"
	"io/fs"
	"os"
	"os/user"
	"path/filepath"
	"regexp"
	"strconv"
	"text/template"
)

type daemonize struct {
	usergroup string
	etc       string
	device    string
	bind      string
	dial      string
}

type info struct {
	Description   string
	Documentation string
	Executable    string
	PID           string
	User          string
	Group         string
	Uid           int
	Gid           int
	LogFiles      []string
}

const serviceTemplate = `[Unit]
Description={{.Description}}
Documentation={{.Documentation}}
After=syslog.target network-online.target
Wants=syslog.target network-online.target

[Service]
Type=simple
ExecStart={{.Executable}}
PIDFile={{.PID}}
User={{.User}}
Group={{.Group}}

[Install]
WantedBy=multi-user.target
`

const logRotateTemplate = `{{range .LogFiles}}{{. }} {{end}}{
    daily
    rotate 30
    compress
        compresscmd /bin/bzip2
        compressext .bz2
        dateext
    missingok
    notifempty
    su uhppoted uhppoted
    postrotate
       /usr/bin/killall -HUP uhppoted-httpd
    endscript
}
`

func makeDaemonize() (daemonize, error) {
	d := daemonize{
		usergroup: "uhppoted:uhppoted",
		etc:       "/usr/local/etc/uhppoted/breakout/rpcd",
		device:    DEFAULT_DEVICE,
		bind:      DEFAULT_BIND,
		dial:      DEFAULT_DIAL,
	}

	flagset := flag.NewFlagSet("daemonize", flag.ExitOnError)
	flagset.StringVar(&d.usergroup, "user", d.usergroup, "user:group for rpcd service")
	flagset.StringVar(&d.device, "device", d.device, "serial device ID")
	flagset.StringVar(&d.bind, "bind", d.bind, "bind address (in the format network::address:port e.g. tcp::0.0.0.0:12345")
	flagset.StringVar(&d.dial, "dial", d.dial, "dial address (for events, in the format network::address:port e.g. tcp::0.0.0.0:12345")
	flagset.Parse(os.Args[2:])

	if d.device == "" {
		return daemonize{}, fmt.Errorf("missing --device arg")
	}

	return d, nil
}

func (d daemonize) exec() error {
	if executable, err := os.Executable(); err != nil {
		return err
	} else if uid, gid, err := getUserGroup(d.usergroup); err != nil {
		fmt.Println()
		fmt.Printf("     **** PLEASE CREATE uid:gid %v (OR SPECIFY A DIFFERENT uid:gid WITH the --user OPTION) ***\n", d.usergroup)
		fmt.Println()
		return err
	} else {
		username := "uhppoted"
		if u, err := user.LookupId(fmt.Sprintf("%v", uid)); err == nil {
			username = u.Username
		}

		fmt.Println()
		fmt.Println("   ... daemonizing")

		info := info{
			Description:   "uhppoted-breakout RPCD service/daemon ",
			Documentation: "https://github.com/uhppoted/uhppoted-breakout",
			Executable:    executable,
			PID:           fmt.Sprintf("/var/uhppoted/breakout/%s.pid", SERVICE),
			User:          "uhppoted",
			Group:         "uhppoted",
			Uid:           uid,
			Gid:           gid,
			LogFiles: []string{
				fmt.Sprintf("/var/log/uhppoted/breakout/%s.log", SERVICE),
			},
		}

		chown := func(path string, info fs.DirEntry, err error) error {
			if err == nil {
				err = os.Chown(path, uid, gid)
			}
			return err
		}

		if err := d.systemd(info); err != nil {
			return err
		}

		if err := d.mkdirs(info); err != nil {
			return err
		}

		if err := d.logrotate(info); err != nil {
			return err
		}

		if err := filepath.WalkDir(d.etc, chown); err != nil {
			return err
		}

		fmt.Printf("   ... %s registered as a systemd service\n", SERVICE)
		fmt.Println()
		fmt.Println("   The daemon will start automatically on the next system restart - to start it manually, execute the following command:")
		fmt.Println()
		fmt.Printf("     > sudo systemctl start  %s\n", SERVICE)
		fmt.Printf("     > sudo systemctl status %s\n", SERVICE)
		fmt.Println()
		fmt.Println("   For some system configurations it may be necessary to also enable the service:")
		fmt.Println()
		fmt.Printf("     > sudo systemctl enable %v\n", SERVICE)
		fmt.Println()
		fmt.Println()
		fmt.Println("   The firewall may need additional rules to allow TCP/IP connections to the service, e.g. for UFW:")
		fmt.Println()
		fmt.Printf("     > sudo ufw allow from any to %v proto tcp\n", d.bind)
		fmt.Println()
		fmt.Printf("   The installation can be verified by running the %v service in 'console' mode:\n", SERVICE)
		fmt.Println()
		fmt.Printf("     > sudo su %v\n", username)
		fmt.Printf("     > ./%v --debug --console\n", SERVICE)
		fmt.Println()

		return nil
	}
}

func (d daemonize) systemd(i info) error {
	service := fmt.Sprintf("%s.service", SERVICE)
	path := filepath.Join("/etc/systemd/system", service)
	t := template.Must(template.New(service).Parse(serviceTemplate))

	fmt.Printf("   ... creating '%s'\n", path)
	f, err := os.OpenFile(path, os.O_WRONLY|os.O_CREATE|os.O_TRUNC, 0644)
	if err != nil {
		return err
	}

	defer f.Close()

	return t.Execute(f, i)
}

func (d daemonize) mkdirs(i info) error {
	directories := []string{
		"/usr/local/etc/uhppoted/breakout/rpcd",
		"/var/uhppoted/breakout/rpcd",
		"/var/log/uhppoted/breakout",
	}

	for _, dir := range directories {
		fmt.Printf("   ... creating '%s'\n", dir)

		if err := os.MkdirAll(dir, 0770); err != nil {
			return err
		}

		if err := os.Chown(dir, i.Uid, i.Gid); err != nil {
			return err
		}
	}

	return nil
}

func (d daemonize) logrotate(i info) error {
	path := filepath.Join("/etc/logrotate.d", SERVICE)
	t := template.Must(template.New(fmt.Sprintf("%s.logrotate", SERVICE)).Parse(logRotateTemplate))

	fmt.Printf("   ... creating '%s'\n", path)
	f, err := os.OpenFile(path, os.O_WRONLY|os.O_CREATE|os.O_TRUNC, 0644)
	if err != nil {
		return err
	}

	defer f.Close()

	return t.Execute(f, i)
}

func getUserGroup(s string) (int, int, error) {
	match := regexp.MustCompile(`(\w+?):(\w+)`).FindStringSubmatch(s)
	if match == nil {
		return 0, 0, fmt.Errorf("invalid user:group '%s'", s)
	}

	u, err := user.Lookup(match[1])
	if err != nil {
		return 0, 0, err
	}

	g, err := user.LookupGroup(match[2])
	if err != nil {
		return 0, 0, err
	}

	uid, err := strconv.Atoi(u.Uid)
	if err != nil {
		return 0, 0, err
	}

	gid, err := strconv.Atoi(g.Gid)
	if err != nil {
		return 0, 0, err
	}

	return uid, gid, nil
}
