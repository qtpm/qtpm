package qtpm

import (
	"github.com/k0kubun/pp"
	"reflect"
	"testing"
)

type TSortTest struct{}

func TestTopologicalSort_1(t *testing.T) {
	edges := [][2]string{
		{"7", "11"},
		{"7", "8"},
		{"5", "11"},
	}
	result, loopEdges := TopologicalSort(edges)
	if !reflect.DeepEqual(result, []string{"7", "5", "8", "11"}) {
		t.Errorf("Result node order is wrong: %s", pp.Sprint(result))
	}
	if len(loopEdges) != 0 {
		t.Errorf("Loop should not be exist in this result, bug %s", pp.Sprint(loopEdges))
	}
}

func TestTopologicalSort_2(t *testing.T) {
	edges := [][2]string{
		{"7", "11"},
		{"7", "8"},
		{"5", "11"},
		{"11", "2"},
		{"11", "9"},
		{"8", "9"},
	}
	result, loopEdges := TopologicalSort(edges)

	if !reflect.DeepEqual(result, []string{"7", "5", "8", "11", "2", "9"}) {
		t.Errorf("Result node order is wrong: %s", pp.Sprint(result))
	}
	if len(loopEdges) != 0 {
		t.Errorf("Loop should not be exist in this result, bug %s", pp.Sprint(loopEdges))
	}
}

func TestTopologicalSortError(t *testing.T) {
	edges := [][2]string{
		{"5", "7"},
		{"7", "11"},
		{"11", "8"},
		{"8", "7"},
	}
	result, loopEdges := TopologicalSort(edges)

	if !reflect.DeepEqual(result, []string{"5"}) {
		t.Errorf("Result node order is wrong: %s", pp.Sprint(result))
	}
	if len(loopEdges) != 3 {
		t.Errorf("Loop should not be exist in this result, bug %s", pp.Sprint(loopEdges))
	}
}
