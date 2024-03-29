CREATE DATABASE BBS;
use bbs;
drop table USER;
drop table BBS;
drop table music;

CREATE TABLE USER (
userID VARCHAR(20),
userPassword VARCHAR(20),
userName VARCHAR(20),
userGender VARCHAR(20),
userEmail VARCHAR(50),
PRIMARY KEY (userID) 
) CHARACTER SET utf8 COLLATE utf8_general_ci; 


CREATE TABLE BBS (
bbsID INT,
bbsTitle VARCHAR(50),
userID VARCHAR(20),
bbsDate DATETIME,
 bbsContent VARCHAR(2048),
bbsAvailable INT,
PRIMARY KEY (bbsID)
) CHARACTER SET utf8 COLLATE utf8_general_ci; 

CREATE TABLE IF NOT EXISTS music(
   id int auto_increment,
   musicname VARCHAR(30),
   mp3 VARCHAR(100),
   singer VARCHAR(30),
   PRIMARY KEY (id)
)default CHARSET=utf8;

select * from USER;
select * from BBS;
select * from music;

show databases
