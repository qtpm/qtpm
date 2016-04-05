// +build !windows

package qtpm

import (
	"bufio"
	"bytes"
	"fmt"
	"io"
	"os"
	"os/exec"
	"sync"
	"syscall"

	"github.com/kr/pty"
	"github.com/mattn/go-colorable"
)

type Cmd struct {
	command *exec.Cmd
	Silent  bool
}

func Command(command, workDir string, args ...string) *Cmd {
	cmd := exec.Command(command, args...)
	cmd.Dir = workDir
	cmd.Env = os.Environ()
	return &Cmd{
		command: cmd,
	}
}

func (c *Cmd) AddEnv(env ...string) {
	c.command.Env = append(c.command.Env, env...)
}
func (c *Cmd) Run() error {
	if Verbose {
		PrintCommand(c.command, c.command.Env[len(os.Environ()):])
	}
	cmd := c.command
	outpty, outtty, err := pty.Open()
	cmd.Stdin = outtty
	cmd.Stdout = outtty

	errpty, errtty, err := pty.Open()
	// stderr needs filtering
	cmd.Stderr = errtty
	if cmd.SysProcAttr == nil {
		cmd.SysProcAttr = &syscall.SysProcAttr{}
	}
	cmd.SysProcAttr.Setctty = true
	cmd.SysProcAttr.Setsid = true

	var outputWait sync.WaitGroup
	outputWait.Add(2)
	var colorableStdout io.Writer
	if c.Silent {
		colorableStdout = bytes.NewBuffer(nil)
	} else {
		colorableStdout = colorable.NewColorableStdout()
	}
	go func() {
		scanner := bufio.NewScanner(outpty)
		for scanner.Scan() {
			fmt.Fprintf(colorableStdout, "%s\n", scanner.Text())
		}
		outputWait.Done()
	}()
	colorableStderr := colorable.NewColorableStderr()
	go func() {
		scanner := bufio.NewScanner(errpty)
		for scanner.Scan() {
			fmt.Fprintf(colorableStderr, "%s\n", scanner.Text())
		}
		outputWait.Done()
	}()
	err = cmd.Start()
	if err != nil {
		return err
	}

	outtty.Close()
	errtty.Close()

	err = cmd.Wait()
	outputWait.Wait()
	return err
}
