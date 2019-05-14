#!/usr/bin/env bash

./client 0 "adminpass123456" 34 0 "2 400 2passwords";
./client 0 "adminpass123456" 34 0 "3 400 3passwords";
./client 3 "3passwords" 34 2 "2 200";