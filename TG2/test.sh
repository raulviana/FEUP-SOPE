#!/usr/bin/env bash

./client 0 "adminpass123456" 34 0 "2 400 2passwords";
./client 0 "adminpass123456" 34 0 "3 400 3passwords";
./client 0 "adminpasswrong" 34 0 "3 400 3passwords";
./client 3 "3passwords" 34 0 "4 400 4passwords";
./client 3 "3passwords" 34 2 "2 200";
./client 0 "adminpass123456" 34 2 "2 200";
./client 3 "3passwordswrong" 34 1 "";
./client 2 "2passwords" 34 1 "";