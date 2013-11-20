CREATE DATABASE emporio;
USE emporio;
#CREATE USER 'emporio'@'localhost' IDENTIFIED BY 'emporio';
GRANT ALL ON emporio.* TO 'emporio'@'localhost' IDENTIFIED BY 'emporio';
source emporium_data.sql;