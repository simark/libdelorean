<?php
	/**
	 * this script simply creates a Graphviz dot file from an history tree
	 * file. *do not* use this with a large history tree file. execute with
	 * no arguments for usage.
	 *
	 * author: Philippe Proulx <philippe.proulx@polymtl.ca>
	 */
	function print_usage() {
		global $argv;
		
		printf("usage: php %s <htf> <dot>\n", $argv[0]);
		echo "  <htf>  history tree file (input)\n";
		echo "  <dot>  dot file (output)\n";
	}
	function read_node($block_data, $max_children) {
		$types = array(
			0 => "int32",
			1 => "str",
			2 => "uint32",
			3 => "float"
		);
		
		// get infos. (common header)
		$chead = array();
		$h = unpack("C", substr($block_data, 0));
		$chead['type'] = $h[1];
		$h = unpack("V", substr($block_data, 1));
		$chead['start'] = $h[1];
		$h = unpack("V", substr($block_data, 9));
		$chead['end'] = $h[1];
		$h = unpack("V", substr($block_data, 17));
		$chead['seq_number'] = $h[1];
		$h = unpack("V", substr($block_data, 21));
		$chead['parent_seq_number'] = $h[1];
		$h = unpack("V", substr($block_data, 25));
		$chead['interval_count'] = $h[1];
		$h = unpack("V", substr($block_data, 29));
		$chead['var_data_offset'] = $h[1];
		$h = unpack("C", substr($block_data, 33));
		$chead['done'] = $h[1];
		
		// get infos. according to type
		$shead = array();
		switch ($chead['type']) {
			case 1:
			$chead['type'] = 'core';
			$h = unpack("V", substr($block_data, 34));
			$shead['extended'] = $h[1];
			$h = unpack("V", substr($block_data, 38));
			$shead['children_count'] = $h[1];
			break;
			
			case 2:
			$chead['type'] = 'leaf';
			break;
			
			default:
			printf("error: unknown type %d\n", $chead['type']);
			break;
		}
		
		// print infos.
		/*printf("\ttype: %s\n", $chead['type']);
		printf("\trange: [%d, %d]\n", $chead['start'], $chead['end']);
		printf("\tseq. number: %d\n", $chead['seq_number']);
		printf("\tparent seq. number: %d\n", $chead['parent_seq_number']);
		printf("\tinterval count: %d\n", $chead['interval_count']);
		printf("\tvar. data offset: %d\n", $chead['var_data_offset']);
		printf("\tdone?: %s\n", $chead['done'] ? 'yes' : 'no');
		if ($chead['type'] == 'core') {
			printf("\textended?: %s\n", $shead['extended'] ? 'yes' : 'no');
			printf("\tchildren count: %d\n", $shead['children_count']);
		}*/
		$ex = "";
		if ($chead['type'] == 'core') {
			$cc = $shead['children_count'];
			$ex = ", $cc children";
		}
		printf("# {%d} from {%d} [%d, %d], %d intervals%s\n", $chead['seq_number'], $chead['parent_seq_number'],
			$chead['start'], $chead['end'], $chead['interval_count'], $ex);
		
		// print children list
		if ($chead['type'] == 'core') {
			for ($i = 0; $i < $shead['children_count']; ++$i) {
				$off_seq = 42 + $i * 4;
				$off_start = 42 + ($max_children * 4) + ($i * 8);
				$h = unpack("V", substr($block_data, $off_seq));
				$seq = $h[1];
				$h = unpack("V", substr($block_data, $off_start));
				$start = $h[1];
				printf("  + {%d} [%d]\n", $seq, $start);
			}
		}
		
		// print intervals
		for ($i = 0; $i < $chead['interval_count']; ++$i) {
			if ($chead['type'] == 'core') {
				$off_head = 42 + ($max_children * 12) + $i * 25;
			} else {
				$off_head = 34 + $i * 25;
			}
			$h = unpack("V", substr($block_data, $off_head + 0));
			$start = $h[1];
			$h = unpack("V", substr($block_data, $off_head + 8));
			$end = $h[1];
			$h = unpack("V", substr($block_data, $off_head + 16));
			$attr = $h[1];
			$h = unpack("C", substr($block_data, $off_head + 20));
			$type = $h[1];
			$h = unpack("V", substr($block_data, $off_head + 21));
			$value = $h[1];
			$td = $type;
			if (array_key_exists($type, $types)) {
				$td = $types[$type];
			}
			printf("  @ %d (%s) [%d, %d] : %d\n", $attr, $td, $start, $end, $value);
		}
		
		// build infos.
		$infos['common'] = $chead;
		$infos['specific'] = $shead;
		
		return $infos;
	}
	function htf2dot($htf_path, $dot_path) {
		// output dot
		$dot = "digraph hf {\n\tsize=\"6,6\";\n\tnode [color=lightblue2, style=filled];\n";
		
		// file => memory
		$fh = fopen($htf_path, "rb");
		$htf = fread($fh, filesize($htf_path));
		fclose($fh);
		
		// get tree header infos.
		$thead = substr($htf, 0, 4096);
		$h = unpack("V", $thead);
		$magic = $h[1];
		$h = unpack("V", substr($thead, 4));
		$major = $h[1];
		$h = unpack("V", substr($thead, 8));
		$minor = $h[1];
		$h = unpack("V", substr($thead, 12));
		$block_size = $h[1];
		$h = unpack("V", substr($thead, 16));
		$max_children = $h[1];
		$h = unpack("V", substr($thead, 20));
		$node_count = $h[1];
		$h = unpack("V", substr($thead, 24));
		$root_seq = $h[1];
		printf("> magic number: %08x\n", $magic);
		echo "> major: $major\n";
		echo "> minor: $minor\n";
		echo "> block size: $block_size\n";
		echo "> max. children: $max_children\n";
		echo "> node count: $node_count\n";
		echo "> root seq. number: $root_seq\n";
		echo "\n";
		
		// read nodes
		for ($i = 0; $i < $node_count; ++$i) {		
			// get node infos.
			$offset = 4096 + $i * $block_size;
			$block_data = substr($htf, $offset, $block_size);
			$ni = read_node($block_data, $max_children);
			echo "\n";
			
			// add to dot
			if ($ni['common']['parent_seq_number'] == -1) {
				// write as single node if parent seq. number is -1 (this is tree root)
				$dot .= sprintf("\n\t\"%d\";", $ni['common']['seq_number']);
			} else {
				$dot .= sprintf("\n\t\"%d\" -> \"%d\";",
					$ni['common']['parent_seq_number'], $ni['common']['seq_number']);
			}
		}
		
		// finish/write output
		$dot .= "\n}\n";
		file_put_contents($dot_path, $dot);
	}
	
	if ($argc != 3) {
		print_usage();
		exit(1);
	}
	htf2dot($argv[1], $argv[2]);
?>
