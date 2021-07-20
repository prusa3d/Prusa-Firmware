#!/bin/bash
#
# lang-add.sh - multi-language support script
#  add new texts from list (lang_add.txt) to all dictionary files
#
# Input files:
#  lang_add.txt
# Updated files:
#  lang_en.txt and all lang_en_xx.txt

# List of supported languages
LANGUAGES="cz de es fr it pl"

# Community languages
LANGUAGES+=" nl" #Dutch


# insert single text to english dictionary
# $1 - text to insert
# $2 - metadata
insert_en()
{
	#replace '[' and ']' in string with '\[' and '\]'
	str=$(echo "$1" | sed "s/\[/\\\[/g;s/\]/\\\]/g")
	# extract english texts, merge new text, grep line number
	ln=$((cat lang_en.txt; echo "$1") | sed "/^$/d;/^#/d" | sort | grep -n "$str" | sed "s/:.*//;q")
	# calculate position for insertion
	ln=$((3*(ln-2)+1))
	[ "$ln" -lt 1 ] && ln=1
	# insert new text
	sed -i "$ln"'i\\' lang_en.txt
	sed -i "$ln"'i\'"$1"'\' lang_en.txt
	sed -i "${ln}i\\#$2" lang_en.txt
}

# insert single text to translated dictionary
# $1 - text to insert
# $2 - suffix
# $3 - metadata
insert_xx()
{
	#replace '[' and ']' in string with '\[' and '\]'
	str=$(echo "$1" | sed "s/\[/\\\[/g;s/\]/\\\]/g")
	# extract english texts, merge new text, grep line number
	ln=$((cat lang_en_$2.txt; echo "$1") | sed "/^$/d;/^#/d" | sed -n 'p;n' | sort | grep -n "$str" | sed "s/:.*//;q")
	# calculate position for insertion
	ln=$((4*(ln-2)+1))
	[ "$ln" -lt 1 ] && ln=1
	# insert new text
	sed -i "$ln"'i\\' lang_en_$2.txt
	sed -i "$ln"'i\"\x00"\' lang_en_$2.txt
	sed -i "$ln"'i\'"$1"'\' lang_en_$2.txt
	sed -i "${ln}i\\#$3" lang_en_$2.txt
}

# find the metadata for the specified string
# TODO: this is unbeliveably crude
# $1 - text to search for
find_metadata()
{
    sed -ne "s^.*\(_[iI]\|ISTR\)($1).*////\(.*\)^\2^p" ../Firmware/*.[ch]* | head -1
}

# check if input file exists
if ! [ -e lang_add.txt ]; then
	echo "file lang_add.txt not found"
	exit 1
fi

cat lang_add.txt | sed 's/^/"/;s/$/"/' | while read new_s; do
	if grep "$new_s" lang_en.txt >/dev/null; then
		echo "text already exist:"
		echo "$new_s"
		echo
	else
		meta=$(find_metadata "$new_s")
		echo "adding text:"
		echo "$new_s ($meta)"
		echo

		insert_en "$new_s" "$meta"
        for lang in $LANGUAGES; do
            insert_xx "$new_s" "$lang" "$meta"
        done
	fi
done

read -t 5
exit 0
