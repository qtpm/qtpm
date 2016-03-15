package qtpm

import (
	"github.com/fatih/color"
)

var (
	printSection    = color.New(color.FgHiCyan, color.Bold).PrintfFunc()
	printSubSection = color.New(color.FgHiBlue, color.Bold).PrintfFunc()
	printSuccess    = color.New(color.FgHiGreen, color.Bold).PrintfFunc()
)
