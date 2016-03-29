package qtpm

func remove(input [][2]string, value [2]string) [][2]string {
	for i, v := range input {
		if v[0] == value[0] && v[1] == value[1] {
			return append(input[:i], input[i+1:]...)
		}
	}
	return input
}

func TopologicalSort(edges [][2]string) ([]string, [][2]string) {
	nodeSet := make(map[string]bool)
	nodes := make([]string, 0)

	for _, edge := range edges {
		for _, node := range edge {
			_, ok := nodeSet[node]
			if !ok {
				nodeSet[node] = true
				nodes = append(nodes, node)
			}
		}
	}

	L := make([]string, 0)
	S := make([]string, 0)

	for _, node := range nodes {
		match := true
		for _, edge := range edges {
			if node == edge[1] {
				match = false
				break
			}
		}
		if match {
			S = append(S, node)
		}
	}

	for len(S) > 0 {
		n := S[0]
		S = S[1:]
		L = append(L, n)
		loopTargetEdges := make([][2]string, 0)
		for _, e := range edges {
			if e[0] == n {
				loopTargetEdges = append(loopTargetEdges, e)
			}
		}

		for _, e := range loopTargetEdges {
			edges = remove(edges, e)
			m := e[1]
			found := false
			for _, me := range edges {
				if me[1] == m {
					found = true
					break
				}
			}
			if !found {
				S = append(S, m)
			}
		}
	}
	return L, edges
}
