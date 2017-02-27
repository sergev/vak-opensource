#!/bin/bash
mkdir certs
rm certs/*
echo "make server cert"
openssl req -new -nodes -x509 -out certs/server.pem -keyout certs/server.key -days 3650 -subj "/C=DE/ST=NRW/L=Earth/O=Golang/OU=IT/CN=www.greynode.org/emailAddress=fake@fake.com"
echo "make client cert"
openssl req -new -nodes -x509 -out certs/client.pem -keyout certs/client.key -days 3650 -subj "/C=DE/ST=NRW/L=Earth/O=Golang/OU=IT/CN=www.greynode.org/emailAddress=fake@fake.com"
