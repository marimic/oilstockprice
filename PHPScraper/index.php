<?php
/**
Oil Price Scraper
Kenny Lozowski - kenny_lozowski@banffcentre.ca
for Maria Michails

Goes to Yahoo and gets oil price -- Yahoo updates once a minute with a 30 minute latency against the market
Uses the CURL library, a simple HTTP client
*/

$curlobj = curl_init(); // initialize the CURL library
$dom = new DOMDocument(); //initializes the XML DOM parser
$fullurl = "https://finance.yahoo.com/q?s=CLQ16.NYM"; //this is the URL to get at yahoo, with the NYMEX crude oil symbol as a query argument
$oilprice_elementid = "yfs_l10_clq16.nym"; //the ID of the DOM element which contains the crude oil price -- **note this may change and break the scraper**
curl_setopt($curlobj, CURLOPT_URL, $fullurl); // add the URL argument to curl
curl_setopt($curlobj, CURLOPT_RETURNTRANSFER, true); //curl should store the returned HTTP result in a long string, not print it
$curlstr = curl_exec($curlobj); //run CURL
curl_close($curlobj); // clean up CURL
@$dom->loadHtml($curlstr); //load the CURL string into the DOM parser
$oilprice = $dom->getElementByID($oilprice_elementid)->textContent; //get the text content of the element enclosing the oil price 
print "~".$oilprice."~"; // print it