#!/bin/sh

WIDTH=200
HEIGHT=150
PAGE=32
ROW=4

header() {
	echo '<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<title>AT&amp;T graphs gallery</title>

<link rel="stylesheet" type="text/css" href="style.css"/>

</head>

<body>

<p>Page: ' > $1

	for ((n=0; n<$total_pages; n++)); do
		echo "<a href=\"page.$n.html\">$n</a>" >> $1
	done

	echo '</p>

<table>
<tr>' >> $1
}

footer() {
	echo '</tr>
</table>

</body>
</html>' >> $1
}


total=`ls $1 | wc -l`
total_pages=`expr \( $total / $PAGE \) + 1`

cnt=0
page=0
for i in `ls $1`; do
	mod=`expr $cnt % $ROW`
	mod2=`expr $cnt % $PAGE`

	if [ $mod2 -eq 0 ]; then
		prev=$file
		file="page.$page.html"
		if [ $cnt -ne 0 ]; then
			footer $prev
		fi
		header $file

		page=`expr $page + 1`
	fi

	if [ $mod -eq 0 -a $mod2 -ne 0 ]; then
		echo "</tr><tr>" >> $file
	fi
	src=`basename $i svg`graphml
	echo "<td><a href=\"$1/$i\"><img src=\"$1/$i\" width=\"$WIDTH\" height=\"$HEIGHT\" alt=\"\"/></a> $src</td>" >> $file

	cnt=`expr $cnt + 1`
done
