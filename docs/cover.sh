#*********************************************************************************************************
# Export Control Marking
#
# EAR ECCN 9D515.a, 9E515.a, License Exception GOV 740.11 (b)(2)(ii):
# This document contains data within the purview of the U.S. Export Administration Regulations (EAR),
# 15 CFR 730-774, as is classified as ECCN 9E515.a. These items are controlled by the U.S. Government 
# and are authorized for export by NASA only to fulfill responsibilities of the parties or of a 
# Cooperating Agency of a NASA Gateway program partner (CSA, ESA, JAXA) and their contractors using 
# License Exception GOV 740.11 (b)(2)(ii) in furtherance of the ISS Intergovernmental Agreement and 
# Gateway MOUs. They may not be resold, transferred, or otherwise disposed of, to any other country
# or to any person other than the authorized ultimate consignee or end-user(s), either in their
# original form or after being incorporated into other items, without first obtaining approval from
# the U.S. government or as otherwise authorized by U.S. law and regulations.
#*********************************************************************************************************

## shell script to generate coverpage for PDF generated using html_to_pdf.sh
## Max Lesser 8/22/2023

## --------------------------------------------------------
## Summary: this file creates a coverpage in html to include in the PDF
##
## Detail: includes HTML to create coverpage of the format: 
## "
## 	Core Flight Software 
## 	<APP-FULL-NAME> <APP-SHORT> 
## 	Application 
## 	Software Design Document 
## 	Section 5 
## "
## where content is center aligned, and displaced 350 px from the top. 
## layout can be changed by edditing the html below
## 
## input argumnets: app_name, as used in html file names 
##
## Output: cover_page.html in html directory, 
##
app_name=$1

if [ -z $1 ]; then
	echo " generates html cover page for use with html_to_pdf.sh. This script is called by html_to_pdf.sh and does not need to be called independently. Argumnets: app name (ex: sch_tt)" 
fi 

case $app_name in 
	"sch_tt") 
	app_full_name="Time Triggered Ethernet Scheduler"
	;;
	"cp") 
	app_full_name="Command Processor" 
	;; 
	"psp") 
	app_full_name="Platform Support Package" 
	;; 
	"sbng") 
	app_full_name="Software Bus Network for Gateway" 
	;; 
	"tte_lib") 
	app_full_name="Time Triggered Ethernet Library"
	;; 
	"tte_mgr") 
	app_full_name="Time Triggered Ethernet Manager" 
	;; 
	"vs") 
	app_full_name="Variable Server" 
	;; 
	*) 
	echo "incorrect input" 
	exit 1 
esac
app_name="${app_name^^}"
  

touch html/cover_page.html

cat > html/cover_page.html << EOF
<style> 
.center { 
	padding: 350px 0; 
	text-align:center;  
}

<!DOCTYPE html>
</style>
<html>
  <head>
    <title>New Page</title>
  </head>
  <body>
    <div class="center">
    <h1>Core Flight Software</h1><br>
    <h1>$app_full_name ($app_name)</h1><br>
    <h1>Application</h1><br>
    <h1>Software Design Document</h1><br>
    <h1>Section 5</h1> 
    <h1>
  </body>
</html>

EOF

