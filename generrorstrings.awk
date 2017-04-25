#!/bin/awk -f
BEGIN {
	print("/* Generated code do not edit */")
	print("char *glerrors[] = {")
}

{
	print("\t["$1"] = \""$1"\",")
}

END {
	print("};")
}
