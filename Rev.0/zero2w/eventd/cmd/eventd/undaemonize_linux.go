package main

import (
	"flag"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
	"syscall"
)

type undaemonize struct {
	etc     string
	workdir string
	logdir  string
}

func makeUndaemonize() (undaemonize, error) {
	u := undaemonize{
		etc:     ETC,
		logdir:  LOGDIR,
		workdir: WORKDIR,
	}

	fs := flag.NewFlagSet("undaemonize", flag.ExitOnError)
	fs.Parse(os.Args[2:])

	return u, nil
}

func (u undaemonize) exec() error {
	fmt.Println("   ... undaemonizing")

	if err := u.systemd(); err != nil {
		return err
	}

	if err := u.logrotate(); err != nil {
		return err
	}

	if err := u.clean(); err != nil {
		return err
	}

	fmt.Printf("   ... %s unregistered as a systemd service\n", SERVICE)
	fmt.Printf(`
       NOTE: configuration files in %s,
             log files in %s,
             and working files in %s,
             were not removed and should be deleted manually
`, u.etc, u.logdir, u.workdir)
	fmt.Println()

	return nil
}

func (u undaemonize) systemd() error {
	path := filepath.Join("/etc/systemd/system", fmt.Sprintf("%s.service", SERVICE))
	_, err := os.Stat(path)
	if err != nil && !os.IsNotExist(err) {
		return err
	}

	if os.IsNotExist(err) {
		fmt.Printf("   ... nothing to do for 'systemd'   (%s does not exist)\n", path)
		return nil
	}

	fmt.Printf("   ... stopping %s service\n", SERVICE)
	command := exec.Command("systemctl", "stop", SERVICE)
	out, err := command.CombinedOutput()
	if strings.TrimSpace(string(out)) != "" {
		fmt.Printf("   > %s\n", out)
	}
	if err != nil {
		return fmt.Errorf("failed to stop '%s' (%v)", SERVICE, err)
	}

	fmt.Printf("   ... reloading daemons\n")
	command = exec.Command("systemctl", "daemon-reload")
	out, err = command.CombinedOutput()
	if strings.TrimSpace(string(out)) != "" {
		fmt.Printf("   > %s\n", out)
	}
	if err != nil {
		return fmt.Errorf("failed to reload services (%v)", err)
	}

	fmt.Printf("   ... removing '%s'\n", path)
	err = os.Remove(path)
	if err != nil {
		return err
	}

	return nil
}

func (u undaemonize) logrotate() error {
	path := filepath.Join("/etc/logrotate.d", SERVICE)

	fmt.Printf("   ... removing '%s'\n", path)

	err := os.Remove(path)
	if err != nil {
		return err
	}

	return nil
}

func (u undaemonize) clean() error {
	files := []string{}

	directories := []string{
		u.logdir,
		u.workdir,
	}

	for _, f := range files {
		fmt.Printf("   ... removing '%s'\n", f)
		if err := os.Remove(f); err != nil && !os.IsNotExist(err) {
			return err
		}
	}

	warnings := []string{}
	for _, dir := range directories {
		fmt.Printf("   ... removing '%s'\n", dir)
		if err := os.Remove(dir); err != nil && !os.IsNotExist(err) {
			patherr, ok := err.(*os.PathError)
			if !ok {
				return err
			}

			syserr, ok := patherr.Err.(syscall.Errno)
			if !ok {
				return err
			}

			if syserr != syscall.ENOTEMPTY {
				return err
			}

			warnings = append(warnings, fmt.Sprintf("could not remove directory '%s' (%v)", dir, syserr))
		}
	}

	if len(warnings) > 0 {
		fmt.Println()
		for _, w := range warnings {
			fmt.Printf("   ... WARNING: %v\n", w)
		}
	}

	return nil
}
