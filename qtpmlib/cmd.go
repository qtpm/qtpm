package qtpm

import (
	"fmt"
	"path/filepath"
	"strings"
)

func CommandWithPath(command, workDir, path string, args ...string) *Cmd {
	cmd := Command(command, workDir, args...)
	if path != "" {
		hasPath := false
		for i, env := range cmd.command.Env {
			if strings.HasPrefix(env, "PATH=") || strings.HasPrefix(env, "Path=") {
				cmd.command.Env[i] = fmt.Sprintf("PATH=%s%c%s", path, filepath.ListSeparator, env[5:])
				hasPath = true
				break
			}
		}
		if !hasPath {
			cmd.command.Env = append(cmd.command.Env, "PATH="+path)
		}
	}
	return cmd
}
