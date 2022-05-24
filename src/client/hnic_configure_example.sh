#!/bin/bash

# Reset/Clear any existing configuration
# hnirrigationd_client --host localhost:8080 --reset-config

# Configure the zones
hnirrigationd_client --host localhost:8080 --create-zone --name "Front Grass East" --sec-per-week 2520 --sec-max-cycle 120 --sec-min-cycle 20 --swid-list "s11"
hnirrigationd_client --host localhost:8080 --create-zone --name "Front Grass West" --sec-per-week 3360 --sec-max-cycle 240 --sec-min-cycle 20 --swid-list "s10"
hnirrigationd_client --host localhost:8080 --create-zone --name "Dog Paddock 1" --sec-per-week 1680 --sec-max-cycle 120 --sec-min-cycle 20 --swid-list "s17"
hnirrigationd_client --host localhost:8080 --create-zone --name "Dog Paddock 2" --sec-per-week 1680 --sec-max-cycle 120 --sec-min-cycle 20 --swid-list "s18"
hnirrigationd_client --host localhost:8080 --create-zone --name "Dog Paddock 3" --sec-per-week 1680 --sec-max-cycle 120 --sec-min-cycle 20 --swid-list "s19"
hnirrigationd_client --host localhost:8080 --create-zone --name "Dog Paddock 4" --sec-per-week 1680 --sec-max-cycle 120 --sec-min-cycle 20 --swid-list "s20"
hnirrigationd_client --host localhost:8080 --create-zone --name "Back Fence West" --sec-per-week 1680 --sec-max-cycle 120 --sec-min-cycle 20 --swid-list "s22"
hnirrigationd_client --host localhost:8080 --create-zone --name "Back Fence East" --sec-per-week 1680 --sec-max-cycle 120 --sec-min-cycle 20 --swid-list "s21"
hnirrigationd_client --host localhost:8080 --create-zone --name "Back Pathway West" --sec-per-week 2520 --sec-max-cycle 120 --sec-min-cycle 20 --swid-list "s3"
hnirrigationd_client --host localhost:8080 --create-zone --name "Back Pathway East" --sec-per-week 2520 --sec-max-cycle 120 --sec-min-cycle 20 --swid-list "s2"
hnirrigationd_client --host localhost:8080 --create-zone --name "North Planter" --sec-per-week 2520 --sec-max-cycle 120 --sec-min-cycle 20 --swid-list "s13"
hnirrigationd_client --host localhost:8080 --create-zone --name "East Planter" --sec-per-week 2520 --sec-max-cycle 120 --sec-min-cycle 20 --swid-list "s12"
hnirrigationd_client --host localhost:8080 --create-zone --name "East Shrubs" --sec-per-week 2520 --sec-max-cycle 120 --sec-min-cycle 20 --swid-list "s16"
hnirrigationd_client --host localhost:8080 --create-zone --name "Southeast Berm" --sec-per-week 2520 --sec-max-cycle 120 --sec-min-cycle 20 --swid-list "s14"
hnirrigationd_client --host localhost:8080 --create-zone --name "Southwest Berm" --sec-per-week 2520 --sec-max-cycle 120 --sec-min-cycle 20 --swid-list "s15"
hnirrigationd_client --host localhost:8080 --create-zone --name "Garden" --sec-per-week 2520 --sec-max-cycle 120 --sec-min-cycle 20 --swid-list "s1"
hnirrigationd_client --host localhost:8080 --create-zone --name "Front Planter" --sec-per-week 2520 --sec-max-cycle 120 --sec-min-cycle 20 --swid-list "s9"

# Add a manual modifier for each zone
hnirrigationd_client --host localhost:8080 --create-modifier --name "z1 Manual Time Adj" --modtype-duration 0 --zoneid-list "z1"
hnirrigationd_client --host localhost:8080 --create-modifier --name "z2 Manual Time Adj" --modtype-duration 0 --zoneid-list "z2"
hnirrigationd_client --host localhost:8080 --create-modifier --name "z3 Manual Time Adj" --modtype-duration 0 --zoneid-list "z3"
hnirrigationd_client --host localhost:8080 --create-modifier --name "z4 Manual Time Adj" --modtype-duration 0 --zoneid-list "z4"
hnirrigationd_client --host localhost:8080 --create-modifier --name "z5 Manual Time Adj" --modtype-duration 0 --zoneid-list "z5"
hnirrigationd_client --host localhost:8080 --create-modifier --name "z6 Manual Time Adj" --modtype-duration 0 --zoneid-list "z6"
hnirrigationd_client --host localhost:8080 --create-modifier --name "z7 Manual Time Adj" --modtype-duration 0 --zoneid-list "z7"
hnirrigationd_client --host localhost:8080 --create-modifier --name "z8 Manual Time Adj" --modtype-duration 0 --zoneid-list "z8"
hnirrigationd_client --host localhost:8080 --create-modifier --name "z9 Manual Time Adj" --modtype-duration 0 --zoneid-list "z9"
hnirrigationd_client --host localhost:8080 --create-modifier --name "z10 Manual Time Adj" --modtype-duration 0 --zoneid-list "z10"
hnirrigationd_client --host localhost:8080 --create-modifier --name "z11 Manual Time Adj" --modtype-duration 0 --zoneid-list "z11"
hnirrigationd_client --host localhost:8080 --create-modifier --name "z12 Manual Time Adj" --modtype-duration 0 --zoneid-list "z12"
hnirrigationd_client --host localhost:8080 --create-modifier --name "z13 Manual Time Adj" --modtype-duration 0 --zoneid-list "z13"
hnirrigationd_client --host localhost:8080 --create-modifier --name "z14 Manual Time Adj" --modtype-duration 0 --zoneid-list "z14"
hnirrigationd_client --host localhost:8080 --create-modifier --name "z15 Manual Time Adj" --modtype-duration 0 --zoneid-list "z15"
hnirrigationd_client --host localhost:8080 --create-modifier --name "z16 Manual Time Adj" --modtype-duration 0 --zoneid-list "z16"
hnirrigationd_client --host localhost:8080 --create-modifier --name "z17 Manual Time Adj" --modtype-duration 0 --zoneid-list "z17"

# Add placement zones
hnirrigationd_client --host localhost:8080 --create-placement --name "Evening" --start-time 20:00:00 --end-time 23:00:00 --rank 1 --day-list "Sunday Monday Tuesday Wednesday Thursday Friday Saturday" --zoneid-list "z1 z2 z3 z4 z5 z6 z7 z8 z9 z10 z11 z12 z13 z14 z15 z16 z17"
hnirrigationd_client --host localhost:8080 --create-placement --name "Morning" --start-time 03:00:00 --end-time 06:00:00 --rank 2 --day-list "Sunday Monday Tuesday Wednesday Thursday Friday Saturday" --zoneid-list "z1 z2 z3 z4 z5 z6 z7 z8 z9 z10 z11 z12 z13 z14 z15 z16 z17"

