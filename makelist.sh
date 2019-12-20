#!/bin/bash

#===============================================================================================================================
#
# Convert the WiiFlow xml databases to csv lists
#
#
# It will output a PLATFORM_list.txt in data folder.
# Those files will be used by the createcat program to generate the WiiFlow categories.
#
# When found in the LuSpace database, the genre will be added.
#
# Alt name is also added when found. It's using a fuzzy search, so probable wrong results sometimes.
# 
# GameID|ScreenScraper Name|ScreenScraper Title|Alternate GameEx Name|Category|Player #|Cooperative|Developper|Publisher|Year
#
#================================================================================================================================

PLATFORM_LIST=( "AMIGA" "AMSTRAD" "APPLE2" "ARCADE" "ATARI2600" "ATARI5200" "ATARI7800" "ATARI800" "ATARIST" "BBCMICRO" "C64" "COCO" "COLECOVISION" "CPS1" "CPS2" "CREATIVISION" "DOS" "DRAGON3264" "FDS" "GAMEWATCH" "GAMEGEAR" "GB" "GBA" "GBC" "INTELLIVISION" "LYNX" "MASTERSYSTEM" "MEGACD" "MEGADRIVE" "MSX" "MSX2" "MSX2+" "MSXTURBO" "N64" "NEOGEO" "NEOPOCKET" "NES" "PC9801" "PCENGINE" "PCENGINECD" "PCFX" "PS1" "SATELLAVIEW" "SCUMMVM" "SG1000" "SGFX" "SUPERNES" "TO5" "VECTREX" "VG5000" "VIDEOPAC" "VIRTUALBOY" "WONDERSWAN" "ZX" )
#PLATFORM_LIST=( "ARCADE" )

#for PLATFORM in "${PLATFORM_LIST[@]}"
#do
	# Replace ; by a comma
#	XMLFILE="./plugins_data/${PLATFORM}/${PLATFORM}.xml"
#	sed -i '/<genre>/s/; /,/g' $XMLFILE
#done
#exit

d1=`date +%s`

for PLATFORM in "${PLATFORM_LIST[@]}"
do

XMLFILE="./plugins_data/${PLATFORM}/${PLATFORM}.xml"
GAMEX_FILE="./plugins_data/${PLATFORM}/${PLATFORM}_GEX.txt"
LUSSPACE_FILE="./plugins_data/${PLATFORM}/${PLATFORM}_LUSS.data"

TITLE_NBR=`grep -c "<game name=" $XMLFILE`
mkdir data
mkdir ./data/${PLATFORM}

echo "Creating data for $PLATFORM ..."

if [[ "$PLATFORM" == "WII" ]]; then
	xmlstarlet sel -T -t -m "(//game)" -v "id" -o "|" -o "|" -v "(.//locale[@lang='EN']/title)" -o "|" -o "|" -v "genre" -o "," \
	--if "contains(languages, 'EN')" -o "English|" --else -o "Non-English|" --break -v "(.//input/@players)" -o "|" \
	--if './/rating/@value=""' -o "|" --else -v "(.//rating/@value)" -o " Rating|" --break -v "developer" -o "|" \
	-v "publisher" -o "|" -v "(.//date/@year)" -n wiitdb.xml > ./data/${PLATFORM}/${PLATFORM}_list.txt
	exit
fi


cat "$LUSSPACE_FILE" | cut -d ";" -f1 | cut -d "(" -f1 | sed -e 's/^ //' -e 's/ $//' > tmpNames.txt
LUSS_NAMES_BUFFER="tmpNames.txt"
GEX_BUFFER="$(cat $GAMEX_FILE; printf a)"
GEX_BUFFER="${GEX_BUFFER%a}"

BUF_SS=`xmlstarlet sel -T -t -m "(//game)" -v "id" -o "|" -v "@name" -o "|" -v "(.//locale[@lang='EN']/title)" -o "|" \
				-v "(.//locale[@lang='EN']/genre)" -o "|" -v "(.//input/@players)" \
				--if "(.//control[@type='coop']/@required)" -o "|Coop|" \
				--else -o "||" --break -v "developer" -o "|" -v "publisher" -o "|" -v "(.//date/@year)" -n $XMLFILE`


rm ./data/${PLATFORM}/${PLATFORM}_list.txt

while IFS= read -r line
do
	ID_SS=`echo $line | cut -d "|" -f1`
	NAME_SS=`echo $line | cut -d "|" -f2`
	TITLE_SS=`echo $line | cut -d "|" -f3`
	GENRE_SS=`echo $line | cut -d "|" -f4`
	PLAYER_SS=`echo $line | cut -d "|" -f5`
	COOP_SS=`echo $line | cut -d "|" -f6`
	DEV_SS=`echo $line | cut -d "|" -f7`
	PUBLISH_SS=`echo $line | cut -d "|" -f8`
	YEAR_SS=`echo $line | cut -d "|" -f9`

	NAME_SEARCH_GEX=`awk -v IGNORECASE=1 -v pattern="$NAME_SS" '$0~pattern' "$GAMEX_FILE" | grep -v "|||" | head -1 | cut -d "|" -f2`

	RESULT_SIMIL=`./simil "$NAME_SS" $LUSS_NAMES_BUFFER`

echo "$ID_SS|$NAME_SS|$TITLE_SS|$NAME_SEARCH_GEX|$GENRE_SS|$PLAYER_SS|$COOP_SS|$DEV_SS|$PUBLISH_SS|$YEAR_SS"

	if [[ -z "${NAME_SEARCH_GEX}" ]];then
		#echo "$NAME_SS:EMPTY!!!"
		NAME_SEARCH_GEX=""

		if [[ -n "$RESULT_SIMIL" ]]; then					
			TMPNAME=`echo "$RESULT_SIMIL" | sed -n '1p' `

			if [ "${TMPNAME,,}" = "${NAME_SS,,}" ] || [ "${TMPNAME,,}" = "${TITLE_SS,,}" ]; then
			#if [ "$TMPNAME" == "$NAME_SS" ] || [ "$TMPNAME" == "$TITLE_SS" ]; then
					NAME_SEARCH_GEX=""
			else
				NAME_SEARCH_GEX="$TMPNAME"
			fi
		fi
	else
		#if [ "$NAME_SEARCH_GEX" == "$NAME_SS" ]; then
		if [ "${NAME_SEARCH_GEX,,}" = "${NAME_SS,,}" ] || [ "${NAME_SEARCH_GEX,,}" = "${TITLE_SS,,}" ]; then
				NAME_SEARCH_GEX=""
		fi
	fi

	GENRE_LUSS=""

	if [[ -z "${GENRE_SS}" ]];then
		GENRE_LUSS=`awk -v IGNORECASE=1 -v pattern="$NAME_SS" '$0~pattern' "$LUSSPACE_FILE" | head -1 | cut -d ";" -f3`

		if [[ -z "${GENRE_LUSS}" ]];then
				#echo "Search similar(No Genre): $RESULT_SIMIL"
				if [[ -n "$RESULT_SIMIL" ]]; then
					PATTERN_SIMIL=`echo "$RESULT_SIMIL" | sed -n '1p' `
					GENRE_LUSS=`awk -v IGNORECASE=1 -v pattern="$PATTERN_SIMIL" '$0~pattern' "$LUSSPACE_FILE" | head -1 | cut -d ";" -f3`
				fi
		fi

	else

		if [[ "${GENRE_SS}" == "Action" ]];then
			GENRE_LUSS=`awk -v IGNORECASE=1 -v pattern="$NAME_SS" '$0~pattern' "$LUSSPACE_FILE" | head -1 | cut -d ";" -f3`
			#echo "Search similar(Action): $RESULT_SIMIL"
			if [[ -z "${GENRE_LUSS}" ]];then
					if [[ -n "$RESULT_SIMIL" ]]; then
						PATTERN_SIMIL=`echo "$RESULT_SIMIL" | sed -n '1p' `
						GENRE_LUSS=`awk -v IGNORECASE=1 -v pattern="$PATTERN_SIMIL" '$0~pattern' "$LUSSPACE_FILE" | head -1 | cut -d ";" -f3`
					fi
			fi
		fi

		if [[ "${PLATFORM}" == "DOS" ]];then
			GENRE_LUSS=`awk -v IGNORECASE=1 -v pattern="$NAME_SS" '$0~pattern' "$LUSSPACE_FILE" | head -1 | cut -d ";" -f3`
			#echo "Search similar(Action): $RESULT_SIMIL"
			if [[ -z "${GENRE_LUSS}" ]];then
					if [[ -n "$RESULT_SIMIL" ]]; then
						PATTERN_SIMIL=`echo "$RESULT_SIMIL" | sed -n '1p' `
						GENRE_LUSS=`awk -v IGNORECASE=1 -v pattern="$PATTERN_SIMIL" '$0~pattern' "$LUSSPACE_FILE" | head -1 | cut -d ";" -f3`
					fi
			fi
		fi
	fi

	if [[ "${GENRE_LUSS}" == "${GENRE_SS}" ]];then
		GENRE_LUSS=""
	else
		GENRE_SS="${GENRE_SS},${GENRE_LUSS}"
	fi

	echo "$ID_SS|$NAME_SS|$TITLE_SS|$NAME_SEARCH_GEX|$GENRE_SS|$PLAYER_SS|$COOP_SS|$DEV_SS|$PUBLISH_SS|$YEAR_SS" >> ./data/${PLATFORM}/${PLATFORM}_list.txt
done < <(printf '%s\n' "$BUF_SS")

done
echo -e "\033[00;31m done in $((`date +%s`-$d1))s\033[00m"
exit

