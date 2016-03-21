package qtpm

import (
	"bufio"
	"bytes"
	"fmt"
	"io"
	"os/exec"
	"sync"
	"syscall"

	"github.com/mattn/go-colorable"
)

type Cmd struct {
	command *exec.Cmd
	Silent  bool
}

func Command(command, workDir string, args ...string) *Cmd {
	cmd := exec.Command(command, args...)
	cmd.Dir = workDir
	return &Cmd{
		command: cmd,
	}
}

func (c *Cmd) AddEnv(env ...string) {
	c.command.Env = append(c.command.Env, env...)
}

func (c *Cmd) Run() error {
	cmd := c.command
	stdout, _ := cmd.StdoutPipe()
	stderr, _ := cmd.StderrPipe()
	cmd.SysProcAttr = &syscall.SysProcAttr{HideWindow: true}

	var outputWait sync.WaitGroup
	outputWait.Add(2)
	var colorableStdout io.Writer
	if c.Silent {
		colorableStdout = bytes.NewBuffer(nil)
	} else {
		colorableStdout = colorable.NewColorableStdout()
	}
	go func() {
		scanner := bufio.NewScanner(stdout)
		for scanner.Scan() {
			fmt.Fprintf(colorableStdout, "%s\n", scanner.Text())
		}
		outputWait.Done()
	}()
	colorableStderr := colorable.NewColorableStderr()
	go func() {
		scanner := bufio.NewScanner(stderr)
		for scanner.Scan() {
			fmt.Fprintf(colorableStderr, "%s\n", scanner.Text())
		}
		outputWait.Done()
	}()
	err := cmd.Start()
	if err != nil {
		return err
	}

	err = cmd.Wait()
	outputWait.Wait()
	return err
}
