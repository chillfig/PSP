##########################################################################################################
# Export Control Marking
#
# EAR ECCN 9E515.a and/or 9E515.f (HALO)
#
# Export Administration Regulations (EAR) Notice
# This document contains information which falls under the purview of the Export Administration Regulations (EAR),
# 15 CFR §730-774 and is export controlled. It may be used only to fulfill responsibilities of the Parties of,
# or a Cooperating Agency of a NASA Gateway Program Partner (CSA, ESA, JAXA, MBRSC) and their contractors in
# furtherance of the Gateway MOUs with ESA, CSA, and Japan and IA with MBRSC. Any use, re-transfer,
# or disclosure to any party for any purpose other than the designated use of fulfilling the responsibilities
# of the Gateway MOUs and IA requires prior U.S. Government authorization.
##########################################################################################################

## shell script to generate PDF from html for SDD section 5 using wkhtmltopdf 
## Max Lesser 8/21/2023

## --------------------------------------------------------
## Summary: this file loops over a dir and makes a PDF out of all HTML files in that DIR
#
## Detail: the script allows you to specify files you want and the beginning and at the end of the PDF, 
## in start_files and end_files respectivly. An exclude files is also provided, any files in the exclude list will be omitted. 
## All files not explicilty mentioned in either list will be inserted 
## between start_files and end_files in the order they occur in the direcotry
## As page titles have an HTML tag wkhtmltopdf does not include the sript also goes through and lowers all heading and title tags by one 
##
## 
## input argumnets: app_name, as used in html file names 
##
## Output: pdf of html files with the name <app_name>_sdd_s5.pdf in the directory this script is called from 
##
## Notes wkhtmltopdf will issues warnings for protocols and non html files not recognized, these can be ignored 
##
## Requires: wkhtmltopdf

if [ -z $1 ]; then 
    echo -e "Generates PDF file from html files. should be called in an apps <docs> directory with existing <html> direcotry ( after <build_dox.sh> has been ran with the html option) Dependencies: wkhtmltopdf parameters: app_name " 
else  
	    

	app_name_in=$1                                             # app name input  

	app_name="${app_name_in//_}"                               # remove underscore from app name to match html naming 

	path=$PWD
	protocol="file://"                                         # protocol for file acces,
		                                                   # only need 2 slashes as 
		                                                   # PWD provides the 3rd slash

	now=$(date +"%H:%M")                                       # current time, for filename use

	prefix="$protocol""$path"                                  # prefix includes the protocol and the pwd
	 

	search_dir="$path""/html"                                  # dir to search for html files
	files=""                                                   # list to hold found html files

	files_nocfs=""                                             # Files not explicilty put at the beginning or end


	num=0                                                      # used as bool to check for specially placed files
##-------------------------------------------------------------------------------------------------------------------## 
	## ----- REPLACE HTML TAGS ----- ## 
	# This function replaces HTML tags in the doxygen generated HTML pages
	# This is done as wkhtmltopdf only includes items with tag <hx> (where x is the heading number) in the ToC 
	# much of the content we want as headings however is defined as HTML page titles 
	# this funtion replaces h4 with h5, h3 with h4, . . . title with h1
	# input: file name, we can specify the full path, to allow running the function in loop as we itterate over the html dir
	replace_html_tags () { 
		## 
	    	file_name="$1"
		
		## --- REPALCE h4 with h5 --- ##
		sed -r -i -e 's|<h4>([0-9]{0,})|<h5>'"${unlockedCount}"'|g' "$file_name"
		sed -r -i -e 's|</h4>([0-9]{0,})|</h5>'"${unlockedCount}"'|g' "$file_name"
		## --- REPALCE h3 with h4 --- ##
		sed -r -i -e 's|<h3>([0-9]{0,})|<h4>'"${unlockedCount}"'|g' "$file_name"
		sed -r -i -e 's|</h3>([0-9]{0,})|</h4>'"${unlockedCount}"'|g' "$file_name"
		## --- REPALCE h2 with h3 --- ##
		sed -r -i -e 's|<h2>([0-9]{0,})|<h3>'"${unlockedCount}"'|g' "$file_name"
		sed -r -i -e 's|</h2>([0-9]{0,})|</h3>'"${unlockedCount}"'|g' "$file_name"
		## --- REPALCE h1 with h2 --- ##
		sed -r -i -e 's|<h1>([0-9]{0,})|<h2>'"${unlockedCount}"'|g' "$file_name"
		sed -r -i -e 's|</h1>([0-9]{0,})|</h2>'"${unlockedCount}"'|g' "$file_name"
		## --- REPALCE title with h1 --- ##
		sed -r -i -e 's|<title>([0-9]{0,})|<h1>'"${unlockedCount}"'|g' "$file_name"
		sed -r -i -e 's|</title>([0-9]{0,})|</h1>'"${unlockedCount}"'|g' "$file_name"
	}
##-------------------------------------------------------------------------------------------------------------------## 
	## ----- START FILES ----- ##
	# put putting "tables" content at the front of the PDF
	# order
	# 1. mission cfg
	# 2. platfrom cfg 
	# 3. events 
	# 4. commands 
	# 5. telemetry 
	# 6. struct
	start_files=()
		
	# "$prefix""/html/cfs""$app_name""misscfg.html"
	# " ""$prefix""/html/cfs""$app_name""platcfg.html"
	# " ""$prefix""/html/cfs""$app_name""event.html"
	# " ""$prefix""/html/cfs""$app_name""cmd.html"
	# " ""$prefix""/html/cfs""$app_name""tlm.html"
	# " ""$prefix""/html/cfs""$app_name""struct.html"
	# " ""$prefix""/html/cfs""$app_name""tbl.html")

	## ----- END FILES -----##
	# putting list of functions, global varibles, etc at the end of the PDF 
	end_files=("$prefix""/html/functions.html"
	" ""$prefix""/html/functions_vars.html"
	" ""$prefix""/html/globals.html"
	" ""$prefix""/html/globals_defs.html"
	" ""$prefix""/html/globals_enum.html"
	" ""$prefix""/html/globals_eval.html"
	" ""$prefix""/html/globals_func.html"
	" ""$prefix""/html/globals_s.html"
	" ""$prefix""/html/globals_g.html"
	" ""$prefix""/html/globals_vars.html")

	## ----- EXCLUDE FILES ----- ## 
	# these files will be excluded from the generated PDF 
	exclude_files=("$prefix""/html/cover_page.html"
	" ""$prefix""/html/cfs""$app_name""con.html"
	" ""$prefix""/html/cfs""$app_name""dg.html"
	" ""$prefix""/html/cfs""$app_name""faq.html"
	" ""$prefix""/html/cfs""$app_name""opr.html"
	" ""$prefix""/html/cfs""$app_name""ovr.html"
	" ""$prefix""/html/cfs""$app_name""req.html"
	" ""$prefix""/html/cfs""$app_name""struct.html"
	" ""$prefix""/html/index.html"
	" ""$prefix""/html/pages.html")
	
    ## remove directory references, as this information is included elsewhere
	rm -rf html/dir*.html

	## ----- SPECIAL FILES -----##
	# all the files we want to specifically place, start and end files concatenated in this case 
	# if you want to add additional categories, add them into this array so they can be found in the loop  
	special_files=("${start_files[@]}" "${end_files[@]}" "${exclude_files[@]}")
##-------------------------------------------------------------------------------------------------------------------## 
	## ----- LOOP OVER FILES IN HTML DIR ----- ##


	for entry in "$search_dir"/*.html 
	do
	    files+=("$protocol""$entry"" ")                        # creates list of all files, protocol prepended  
	    current_file="$protocol""$entry"
	    replace_html_tags "$entry"                             # replace html tags for better display in ToC, see function definition above for details 
	    ## --- LOOP OVER SPECIAL FILES --- ## 
	    # loop over special files, and set a flag to ignore any 
	    # file whose location we explicilty want to set
	    # see special files above for the list of files filtered out here 
	    for item in "${special_files[@]}"; do
		if [[ $item == *"$current_file"* ]]; then
		    num=1 
		    break
		fi 
	    done
	    ## --- HANDLE NON_SPECIAL FILES --- ## 
	    # all files not filtered out above will be sorted into the body 
	    # in order of apperance here
	    if (( num == 0 )); then
		files_nocfs+=("$protocol""$entry"" ")
	    fi 
	    num=0
	done


	echo "Files in this report" 
	echo
	echo "start files"
	echo "${start_files[@]}" 
	echo 
	echo "body files" 
	echo "${files_nocfs[@]}"
	echo 
	echo "end files"
	echo "${end_files[@]}" 
	echo 
	echo "excluded files" 
	echo "${exclude_files[@]}"
##-------------------------------------------------------------------------------------------------------------------## 	
	## ----- MAKE COVER PAGE ----- ## 
	# this section calls a script to create the cover page for the pdf and writes it to html/cover_page.html 
	# the look of this page can be changed by edditing the script <cover.sh> 
	./cover.sh $app_name_in

	## ----- ISSUE WKHTMLTOPDF COMMAND ----- ##
	# PDF will be created in the order 
	# start_files - body_files - end_files 
	# if you wish to modify this order, you can simply add or remove files from the start and end files lists at the top 
	# The command to create a PDF in direcotry order is below, uncomment if if that is what you need 
##-------------------------------------------------------------------------------------------------------------------## 	   
	wkhtmltopdf --javascript-delay 5000 --outline-depth 3 --user-style-sheet "$prefix""/html/doxygen.css" --load-error-handling ignore cover html/cover_page.html toc --toc-level-indentation 40  "${start_files[@]}" "${files_nocfs[@]}" "${end_files[@]}" "$app_name_in"_sdd_s5.pdf

	##---------------------------------------------------------
	## List of options and explanation 
	## Note that global options must preceed style options such as "toc", otherwise they will be ignored 
	## --user-style-sheet <PATH> enables use of user supplied .css stylesheet. <PATH> is the path to the sytlesheet
	##-----
	## --load-error-handling ignore
	# allows rendering incase of protocl errors that may occure from list generation 
	##-----
	## toc 
	# Generates table of content 
	## ----- 
	## <PATH TO HTML FILES> 
	# the next argument(s) specify the html input files, they will be combined into the PDF in the order specifed. 
	# paths should be absolute and include the protocol (file:/// in our case) 
	## -----
	## javascript-delay <VALUE> 
	## this command allows a delay to apply any javascripts
	## THIS IS CRITICAL AS OTHERWISE THE PDF WILL NOT RENDER CORRECTLY 
	## -----
	## <OUTPUT PDF> 
	# path to (and name of) the output PDF
	## -----
	## outline depth and toc-level-indentation are supposed to controll the level of headings included in TOC, and the indentation, but seem to have no effect 




	## Command to generate unordered PDF, places items in the order they appear in the /html/ dir
	#wkhtmltopdf --title "SCH_TT SDD S5" --user-style-sheet "$prefix""/html/doxygen.css" --load-error-handling ignore toc "${files[@]}" sch_tt_sdd_s5_"$now".pdf

	# Make PDF with "start_files" only 
	#wkhtmltopdf --user-style-sheet "$prefix""/html/doxygen.css" --load-error-handling ignore toc "${start_files[@]}" sch_tt_sdd_s5_cfs_"$now".pdf
fi






