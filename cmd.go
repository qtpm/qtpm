package main

import (
	"github.com/kr/pty"
	"github.com/mattn/go-colorable"
	"io"
	"os/exec"
	"sync"
	"syscall"
)

type Cmd struct {
	command *exec.Cmd
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
	go func() {
		io.Copy(colorable.NewColorableStdout(), outpty)
		outputWait.Done()
	}()
	go func() {
		io.Copy(colorable.NewColorableStderr(), errpty)
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
	return nil
}
