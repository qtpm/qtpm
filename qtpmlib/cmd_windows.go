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
	stdout, _ := cmd.StdoutPipe()
	stderr, _ := cmd.StderrPipe()
	cmd.SysProcAttr = &syscall.SysProcAttr{HideWindow: true}

	var outputWait sync.WaitGroup
	var lock sync.Mutex
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
			lock.Lock()
			fmt.Fprintf(colorableStdout, "%s\n", scanner.Text())
			lock.Unlock()
		}
		outputWait.Done()
	}()
	colorableStderr := colorable.NewColorableStderr()
	go func() {
		scanner := bufio.NewScanner(stderr)
		for scanner.Scan() {
			lock.Lock()
			fmt.Fprintf(colorableStderr, "%s\n", scanner.Text())
			lock.Unlock()
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
