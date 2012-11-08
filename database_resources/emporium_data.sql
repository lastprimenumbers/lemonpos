-- MySQL dump 10.13  Distrib 5.5.24, for debian-linux-gnu (x86_64)
--
-- Host: localhost    Database: lemondb
-- ------------------------------------------------------
-- Server version	5.5.24-0ubuntu0.12.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `balances`
--

DROP TABLE IF EXISTS `balances`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `balances` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `datetime_start` datetime NOT NULL DEFAULT '2009-01-01 00:00:00',
  `datetime_end` datetime NOT NULL DEFAULT '2009-01-01 00:00:00',
  `userid` bigint(20) unsigned NOT NULL,
  `usern` varchar(50) NOT NULL,
  `initamount` double NOT NULL,
  `in` double NOT NULL,
  `out` double NOT NULL,
  `cash` double NOT NULL,
  `card` double NOT NULL,
  `transactions` varchar(1000) NOT NULL,
  `terminalnum` bigint(20) unsigned NOT NULL,
  `cashflows` varchar(1000) DEFAULT '',
  `done` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `SEC` (`datetime_start`,`datetime_end`,`userid`)
) ENGINE=MyISAM AUTO_INCREMENT=52 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `balances`
--

LOCK TABLES `balances` WRITE;
/*!40000 ALTER TABLE `balances` DISABLE KEYS */;
INSERT INTO `balances` VALUES (1,'2012-01-23 22:26:15','2012-01-23 22:26:15',1,'admin',0,0,0,0,0,'',1,'',0),(2,'2012-01-23 22:26:15','2012-01-23 22:26:15',1,'admin',0,0,0,0,0,'',1,'',0),(3,'2012-01-23 22:26:15','2012-01-23 22:26:15',1,'admin',0,0,0,0,0,'',1,'',0),(4,'2012-01-23 22:28:15','2012-01-23 22:28:36',1,'admin',100,0,0,100,0,'',1,NULL,1),(5,'2012-01-23 22:29:38','2012-01-23 22:31:45',1,'admin',1,0,0,1,0,'',1,NULL,1),(6,'2012-01-23 22:35:20','2012-01-23 22:36:52',1,'admin',0,2,0,2,0,'3',1,NULL,1),(7,'2012-01-23 22:38:03','2012-01-23 22:38:17',1,'admin',1,0,0,1,0,'',1,NULL,1),(8,'2012-01-24 11:53:05','2012-01-24 11:53:05',1,'admin',0,0,0,0,0,'',1,NULL,0),(9,'2012-05-14 21:40:36','2012-05-14 21:41:36',2,'daniele',100,0,0,100,0,'',1,NULL,1),(10,'2012-05-14 22:21:15','2012-05-14 22:21:15',2,'daniele',0,0,0,0,0,'',1,'',0),(11,'2012-05-14 22:44:06','2012-05-14 22:44:07',2,'daniele',0,0,0,0,0,'',1,NULL,0),(12,'2012-05-14 23:26:47','2012-05-14 23:26:47',2,'daniele',0,0,0,0,0,'',1,'',0),(13,'2012-06-15 04:38:15','2012-06-15 04:40:03',2,'daniele',0,0,0,0,33,'20',1,NULL,1),(14,'2012-06-15 04:40:18','2012-06-15 04:43:01',2,'daniele',0,0,0,0,3,'22',1,NULL,1),(15,'2012-06-15 04:44:31','2012-06-15 04:44:31',2,'daniele',0,0,0,0,0,'',1,'',0),(16,'2012-06-15 04:46:01','2012-06-15 04:47:58',2,'daniele',0,0,0,0,18,'25',1,NULL,1),(17,'2012-06-15 04:50:52','2012-06-15 04:50:52',2,'daniele',0,0,0,0,0,'',1,'',0),(18,'2012-06-15 05:03:49','2012-06-15 05:03:49',2,'daniele',0,0,0,0,0,'',1,'',0),(19,'2012-06-15 16:31:19','2012-06-15 16:32:53',2,'daniele',0,0,0,0,0,'',1,NULL,0),(20,'2012-06-15 22:16:24','2012-06-15 22:16:24',2,'daniele',0,0,0,0,0,'',1,'',0),(21,'2012-06-15 22:26:35','2012-06-15 22:28:33',2,'daniele',0,0,0,0,0,'',1,NULL,0),(22,'2012-06-15 22:29:51','2012-06-15 22:30:13',2,'daniele',0,0,0,0,0,'',1,NULL,0),(23,'2012-06-15 22:31:01','2012-06-15 22:31:05',2,'daniele',0,0,0,0,0,'',1,NULL,0),(24,'2012-06-15 22:32:52','2012-06-15 22:32:56',2,'daniele',0,0,0,0,0,'',1,NULL,0),(25,'2012-06-15 22:36:12','2012-06-15 22:36:19',2,'daniele',0,0,0,0,0,'',1,NULL,0),(26,'2012-06-15 22:41:10','2012-06-15 22:41:10',2,'daniele',0,0,0,0,0,'',1,'',0),(27,'2012-06-16 00:36:43','2012-06-16 00:36:44',2,'daniele',0,0,0,0,0,'',1,NULL,0),(28,'2012-06-16 01:06:55','2012-06-16 01:07:30',2,'daniele',0,0,0,0,4,'38',1,NULL,1),(29,'2012-07-02 22:23:11','2012-07-02 22:23:24',2,'daniele',0,0,0,0,0,'',1,NULL,0),(30,'2012-07-02 22:25:25','2012-07-02 22:25:28',2,'daniele',0,0,0,0,0,'',1,NULL,0),(31,'2012-07-18 15:18:57','2012-07-18 15:19:14',2,'daniele',0,0,0,0,0,'',1,NULL,0),(32,'2012-07-18 15:24:27','2012-07-18 15:24:31',2,'daniele',0,0,0,0,0,'',1,NULL,0),(33,'2012-07-18 15:26:26','2012-07-18 15:26:27',2,'daniele',0,0,0,0,0,'',1,NULL,0),(34,'2012-07-18 15:27:21','2012-07-18 15:27:24',2,'daniele',0,0,0,0,0,'',1,NULL,0),(35,'2012-07-18 15:31:26','2012-07-18 15:31:27',2,'daniele',0,0,0,0,0,'',1,NULL,0),(36,'2012-07-18 15:35:31','2012-07-18 15:35:32',2,'daniele',0,0,0,0,0,'',1,NULL,0),(37,'2012-07-18 15:40:02','2012-07-18 15:40:19',2,'daniele',0,0,0,0,60,'48',1,NULL,1),(38,'2012-07-18 15:40:35','2012-07-18 15:40:53',2,'daniele',0,0,0,0,45,'50',1,NULL,1),(39,'2012-07-18 15:41:41','2012-07-18 15:41:43',2,'daniele',0,0,0,0,0,'',1,NULL,0),(40,'2012-07-18 15:57:34','2012-07-18 15:57:36',2,'daniele',0,0,0,0,0,'',1,NULL,0),(41,'2012-07-20 22:18:22','2012-07-20 22:18:24',2,'daniele',0,0,0,0,0,'',1,NULL,0),(42,'2012-07-20 22:23:18','2012-07-20 22:23:20',2,'daniele',0,0,0,0,0,'',1,NULL,0),(43,'2012-07-20 22:25:11','2012-07-20 22:25:20',2,'daniele',0,0,0,0,0,'',1,NULL,0),(44,'2012-07-20 22:28:04','2012-07-20 22:28:09',2,'daniele',0,0,0,0,0,'',1,NULL,0),(45,'2012-07-20 22:40:13','2012-07-20 22:40:15',2,'daniele',0,0,0,0,0,'',1,NULL,0),(46,'2012-07-20 22:43:03','2012-07-20 22:43:11',2,'daniele',0,0,0,0,0,'',1,NULL,0),(47,'2012-07-23 20:05:32','2012-07-23 20:05:32',2,'daniele',0,0,0,0,0,'',1,'',0),(48,'2012-09-17 21:46:57','2012-09-17 21:47:30',2,'daniele',0,0,0,0,0,'',1,NULL,0),(49,'2012-09-17 22:40:34','2012-09-17 22:41:36',2,'daniele',0,0,0,0,0,'',1,NULL,0),(50,'2012-09-17 23:17:53','2012-09-17 23:17:53',2,'daniele',0,0,0,0,0,'',1,'',0),(51,'2012-09-17 23:19:50','2012-09-17 23:19:50',2,'daniele',0,0,0,0,0,'',1,'',0);
/*!40000 ALTER TABLE `balances` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `bool_values`
--

DROP TABLE IF EXISTS `bool_values`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bool_values` (
  `id` int(10) unsigned NOT NULL DEFAULT '0',
  `text` varchar(50) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `bool_values`
--

LOCK TABLES `bool_values` WRITE;
/*!40000 ALTER TABLE `bool_values` DISABLE KEYS */;
INSERT INTO `bool_values` VALUES (0,'NO'),(1,'YES');
/*!40000 ALTER TABLE `bool_values` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cashflow`
--

DROP TABLE IF EXISTS `cashflow`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cashflow` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `type` smallint(5) unsigned NOT NULL DEFAULT '1',
  `userid` bigint(20) NOT NULL DEFAULT '1',
  `reason` varchar(255) DEFAULT '',
  `amount` double unsigned NOT NULL DEFAULT '0',
  `date` date NOT NULL DEFAULT '2009-01-01',
  `time` time NOT NULL DEFAULT '00:00:00',
  `terminalnum` int(10) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`),
  KEY `SEC` (`date`,`time`,`type`,`userid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cashflow`
--

LOCK TABLES `cashflow` WRITE;
/*!40000 ALTER TABLE `cashflow` DISABLE KEYS */;
/*!40000 ALTER TABLE `cashflow` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cashflowtypes`
--

DROP TABLE IF EXISTS `cashflowtypes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cashflowtypes` (
  `typeid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `text` varchar(50) NOT NULL,
  PRIMARY KEY (`typeid`)
) ENGINE=MyISAM AUTO_INCREMENT=9 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cashflowtypes`
--

LOCK TABLES `cashflowtypes` WRITE;
/*!40000 ALTER TABLE `cashflowtypes` DISABLE KEYS */;
INSERT INTO `cashflowtypes` VALUES (1,'Normal cash OUT'),(2,'Money return on ticket cancel'),(3,'Money return on product return'),(4,'Normal Cash IN'),(5,'Cash IN Reservation'),(6,'Cash OUT Reservation'),(7,'Cash IN Credit Payment'),(8,'Cash IN Debit');
/*!40000 ALTER TABLE `cashflowtypes` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `categories`
--

DROP TABLE IF EXISTS `categories`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `categories` (
  `catid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `text` varchar(50) NOT NULL,
  PRIMARY KEY (`catid`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `categories`
--

LOCK TABLES `categories` WRITE;
/*!40000 ALTER TABLE `categories` DISABLE KEYS */;
INSERT INTO `categories` VALUES (1,'General');
/*!40000 ALTER TABLE `categories` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `clients`
--

DROP TABLE IF EXISTS `clients`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `clients` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `code` varchar(13) DEFAULT '0000000000000',
  `name` varchar(255) DEFAULT '',
  `since` date NOT NULL DEFAULT '2009-01-01',
  `expiry` date NOT NULL DEFAULT '2012-12-01',
  `address` varchar(255) DEFAULT NULL,
  `phone` varchar(50) DEFAULT NULL,
  `phone_movil` varchar(50) DEFAULT NULL,
  `points` bigint(20) unsigned DEFAULT '0',
  `monthly` double NOT NULL DEFAULT '0',
  `discount` double NOT NULL,
  `photo` blob,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `clients`
--

LOCK TABLES `clients` WRITE;
/*!40000 ALTER TABLE `clients` DISABLE KEYS */;
INSERT INTO `clients` VALUES (1,'0000000000000','General','2009-01-01','2012-11-05','','','',480,5,0,NULL),(2,'123','Pepito Sbazzeguti','2012-06-13','2014-06-13','','','',690,30,0,NULL);
/*!40000 ALTER TABLE `clients` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `config`
--

DROP TABLE IF EXISTS `config`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `config` (
  `firstrun` varchar(30) NOT NULL,
  `taxIsIncludedInPrice` tinyint(1) NOT NULL DEFAULT '1',
  `storeLogo` blob,
  `storeName` varchar(255) DEFAULT NULL,
  `storeAddress` varchar(255) DEFAULT NULL,
  `storePhone` varchar(100) DEFAULT NULL,
  `logoOnTop` tinyint(1) NOT NULL DEFAULT '1',
  `useCUPS` tinyint(1) NOT NULL DEFAULT '1',
  `smallPrint` tinyint(1) NOT NULL DEFAULT '1',
  `db_version` varchar(10) DEFAULT NULL,
  PRIMARY KEY (`firstrun`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `config`
--

LOCK TABLES `config` WRITE;
/*!40000 ALTER TABLE `config` DISABLE KEYS */;
INSERT INTO `config` VALUES ('yes, it is February 6 1978',1,'','Lemon Store','','',1,1,1,'0950');
/*!40000 ALTER TABLE `config` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `credit_history`
--

DROP TABLE IF EXISTS `credit_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `credit_history` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `customerid` bigint(20) unsigned NOT NULL,
  `saleid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `amount` double NOT NULL DEFAULT '0',
  `date` date NOT NULL,
  `time` time NOT NULL,
  PRIMARY KEY (`id`),
  KEY `SEC` (`customerid`)
) ENGINE=MyISAM AUTO_INCREMENT=7 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `credit_history`
--

LOCK TABLES `credit_history` WRITE;
/*!40000 ALTER TABLE `credit_history` DISABLE KEYS */;
INSERT INTO `credit_history` VALUES (1,1,20,33,'2012-06-15','04:39:49'),(2,1,22,3,'2012-06-15','04:42:22'),(3,2,25,18,'2012-06-15','04:46:38'),(4,2,38,4,'2012-06-16','01:07:05'),(5,2,48,60,'2012-07-18','15:40:10'),(6,2,50,45,'2012-07-18','15:40:41');
/*!40000 ALTER TABLE `credit_history` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `credits`
--

DROP TABLE IF EXISTS `credits`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `credits` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `customerid` bigint(20) unsigned NOT NULL,
  `total` double NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `SEC` (`customerid`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `credits`
--

LOCK TABLES `credits` WRITE;
/*!40000 ALTER TABLE `credits` DISABLE KEYS */;
INSERT INTO `credits` VALUES (1,1,36),(2,2,127);
/*!40000 ALTER TABLE `credits` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `currencies`
--

DROP TABLE IF EXISTS `currencies`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `currencies` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(512) DEFAULT NULL,
  `factor` double NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=12 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `currencies`
--

LOCK TABLES `currencies` WRITE;
/*!40000 ALTER TABLE `currencies` DISABLE KEYS */;
INSERT INTO `currencies` VALUES (1,'â‚¬',1),(2,'U',1);
/*!40000 ALTER TABLE `currencies` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `logs`
--

DROP TABLE IF EXISTS `logs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `logs` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `userid` bigint(20) unsigned NOT NULL,
  `date` varchar(20) NOT NULL DEFAULT '2009-01-01',
  `time` varchar(20) NOT NULL DEFAULT '00:00',
  `action` varchar(512) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=56 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `logs`
--

LOCK TABLES `logs` WRITE;
/*!40000 ALTER TABLE `logs` DISABLE KEYS */;
INSERT INTO `logs` VALUES (1,1,'2012-01-23','22:28','[ LEMON ] Operation Started by Administrator at terminal 1'),(2,1,'2012-01-23','22:28','[ LEMON ] End of Day report printed by Administrator at terminal 1 on 2012-01-23 22:28'),(3,1,'2012-01-23','22:29','[ LEMON ] Operation Started by Administrator at terminal 1'),(4,1,'2012-01-23','22:35','[ LEMON ] Operation Started by Administrator at terminal 1'),(5,1,'2012-01-23','22:38','[ LEMON ] Operation Started by Administrator at terminal 1'),(6,1,'2012-01-24','11:53','[ LEMON ] Operation Started by Administrator at terminal 1'),(7,2,'2012-05-14','21:40','[ LEMON ] Operation Started by Administrator at terminal 1'),(8,1,'2012-05-14','22:04','[SQUEEZE] Purchase #15 - 2 x Ciao2 (1.111)'),(9,2,'2012-05-14','22:21','[ LEMON ] Operation Started by Administrator at terminal 1'),(10,2,'2012-05-14','22:44','[ LEMON ] Operation Started by Administrator at terminal 1'),(11,2,'2012-05-14','23:26','[ LEMON ] Operation Started by Administrator at terminal 1'),(12,2,'2012-06-15','04:38','[ LEMON ] Operation Started by Administrator at terminal 1'),(13,2,'2012-06-15','04:40','[ LEMON ] Operation Started by Administrator at terminal 1'),(14,1,'2012-06-15','04:43','[SQUEEZE] Stock Correction: [Product 1.111] from 0 to 100. Reason:add'),(15,1,'2012-06-15','04:43','[SQUEEZE] Stock Correction: [Product 4.950.232.123.566] from 0 to 200. Reason:add'),(16,2,'2012-06-15','04:44','[ LEMON ] Operation Started by Administrator at terminal 1'),(17,2,'2012-06-15','04:46','[ LEMON ] Operation Started by Administrator at terminal 1'),(18,2,'2012-06-15','04:50','[ LEMON ] Operation Started by Administrator at terminal 1'),(19,2,'2012-06-15','05:03','[ LEMON ] Operation Started by Administrator at terminal 1'),(20,2,'2012-06-15','16:31','[ LEMON ] Operation Started by Administrator at terminal 1'),(21,2,'2012-06-15','22:16','[ LEMON ] Operation Started by Administrator at terminal 1'),(22,2,'2012-06-15','22:26','[ LEMON ] Operation Started by Administrator at terminal 1'),(23,2,'2012-06-15','22:29','[ LEMON ] Operation Started by Administrator at terminal 1'),(24,2,'2012-06-15','22:31','[ LEMON ] Operation Started by Administrator at terminal 1'),(25,2,'2012-06-15','22:32','[ LEMON ] Operation Started by Administrator at terminal 1'),(26,2,'2012-06-15','22:36','[ LEMON ] Operation Started by Administrator at terminal 1'),(27,2,'2012-06-15','22:41','[ LEMON ] Operation Started by Administrator at terminal 1'),(28,2,'2012-06-16','00:36','[ LEMON ] Operation Started by Administrator at terminal 1'),(29,2,'2012-06-16','01:06','[ LEMON ] Operation Started by Administrator at terminal 1'),(30,2,'2012-07-02','22:23','[ LEMON ] Operation Started by Administrator at terminal 1'),(31,2,'2012-07-02','22:25','[ LEMON ] Operation Started by Administrator at terminal 1'),(32,2,'2012-07-18','15:18','[ LEMON ] Operation Started by Administrator at terminal 1'),(33,2,'2012-07-18','15:24','[ LEMON ] Operation Started by Administrator at terminal 1'),(34,2,'2012-07-18','15:26','[ LEMON ] Operation Started by Administrator at terminal 1'),(35,2,'2012-07-18','15:27','[ LEMON ] Operation Started by Administrator at terminal 1'),(36,2,'2012-07-18','15:31','[ LEMON ] Operation Started by Administrator at terminal 1'),(37,2,'2012-07-18','15:35','[ LEMON ] Operation Started by Administrator at terminal 1'),(38,2,'2012-07-18','15:40','[ LEMON ] Operation Started by Administrator at terminal 1'),(39,2,'2012-07-18','15:40','[ LEMON ] Operation Started by Administrator at terminal 1'),(40,2,'2012-07-18','15:41','[ LEMON ] Operation Started by Administrator at terminal 1'),(41,2,'2012-07-18','15:57','[ LEMON ] Operation Started by Administrator at terminal 1'),(42,2,'2012-07-20','22:18','[ LEMON ] Operation Started by Administrator at terminal 1'),(43,2,'2012-07-20','22:23','[ LEMON ] Operation Started by Administrator at terminal 1'),(44,2,'2012-07-20','22:25','[ LEMON ] Operation Started by Administrator at terminal 1'),(45,2,'2012-07-20','22:28','[ LEMON ] Operation Started by Administrator at terminal 1'),(46,2,'2012-07-20','22:40','[ LEMON ] Operation Started by Administrator at terminal 1'),(47,2,'2012-07-20','22:43','[ LEMON ] Operation Started by Administrator at terminal 1'),(48,2,'2012-07-23','20:05','[ LEMON ] Operation Started by Administrator at terminal 1'),(49,1,'2012-07-23','20:45','[SQUEEZE] Purchase #61 - 100 x Ciao2 (1.111)'),(50,1,'2012-09-17','21:30','[SQUEEZE] Purchase #62 - 0 x sdrgs (12)'),(51,1,'2012-09-17','21:31','[SQUEEZE] Purchase #63 - 0 x sdrfsr (213)'),(52,2,'2012-09-17','21:46','[ LEMON ] Operation Started by Administrator at terminal 1'),(53,2,'2012-09-17','22:40','[ LEMON ] Operation Started by Administrator at terminal 1'),(54,2,'2012-09-17','23:17','[ LEMON ] Operation Started by Administrator at terminal 1'),(55,2,'2012-09-17','23:19','[ LEMON ] Operation Started by Administrator at terminal 1');
/*!40000 ALTER TABLE `logs` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `measures`
--

DROP TABLE IF EXISTS `measures`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `measures` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `text` varchar(50) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `measures`
--

LOCK TABLES `measures` WRITE;
/*!40000 ALTER TABLE `measures` DISABLE KEYS */;
INSERT INTO `measures` VALUES (1,'Pc');
/*!40000 ALTER TABLE `measures` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `offers`
--

DROP TABLE IF EXISTS `offers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `offers` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `discount` double NOT NULL,
  `datestart` date NOT NULL DEFAULT '2009-01-01',
  `dateend` date NOT NULL DEFAULT '2009-01-01',
  `product_id` bigint(20) unsigned NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `offers`
--

LOCK TABLES `offers` WRITE;
/*!40000 ALTER TABLE `offers` DISABLE KEYS */;
/*!40000 ALTER TABLE `offers` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `paytypes`
--

DROP TABLE IF EXISTS `paytypes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `paytypes` (
  `typeid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `text` varchar(50) NOT NULL,
  PRIMARY KEY (`typeid`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `paytypes`
--

LOCK TABLES `paytypes` WRITE;
/*!40000 ALTER TABLE `paytypes` DISABLE KEYS */;
INSERT INTO `paytypes` VALUES (1,'Cash'),(2,'Card');
/*!40000 ALTER TABLE `paytypes` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `products`
--

DROP TABLE IF EXISTS `products`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `products` (
  `code` bigint(20) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL DEFAULT 'unknown',
  `price` double unsigned NOT NULL DEFAULT '0',
  `stockqty` double NOT NULL DEFAULT '0',
  `cost` double unsigned NOT NULL DEFAULT '0',
  `soldunits` double unsigned NOT NULL DEFAULT '0',
  `datelastsold` date DEFAULT '2009-01-01',
  `units` int(10) unsigned NOT NULL DEFAULT '0',
  `taxpercentage` double unsigned NOT NULL DEFAULT '15',
  `extrataxes` double unsigned NOT NULL DEFAULT '0',
  `photo` blob,
  `category` int(10) unsigned NOT NULL DEFAULT '0',
  `points` int(10) unsigned NOT NULL DEFAULT '0',
  `alphacode` varchar(30) DEFAULT NULL,
  `lastproviderid` int(10) unsigned NOT NULL DEFAULT '1',
  `isARawProduct` tinyint(1) NOT NULL DEFAULT '0',
  `isAGroup` tinyint(1) NOT NULL DEFAULT '0',
  `groupElements` varchar(1000) DEFAULT '',
  `groupPriceDrop` double unsigned NOT NULL DEFAULT '0',
  `taxmodel` bigint(20) unsigned NOT NULL DEFAULT '1',
  `hasUnlimitedStock` tinyint(1) NOT NULL DEFAULT '0',
  `isNotDiscountable` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`code`),
  KEY `SEC` (`category`,`name`,`alphacode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `products`
--

LOCK TABLES `products` WRITE;
/*!40000 ALTER TABLE `products` DISABLE KEYS */;
INSERT INTO `products` VALUES (4950232123566,'Ciao',2,198,1,3,'2012-06-16',1,0,0,NULL,1,10,'-NA-',1,0,0,'',0,1,0,0),(2345,'sdfsdf',3.03,82,3,3,'2012-06-15',1,1,0,NULL,1,0,'-NA-',1,0,0,'',0,1,0,0),(1111,'Ciao2',15,192,12,10,'2012-07-18',1,0,0,NULL,1,10,'aereg',1,0,0,'',0,1,0,0),(213,'sdrfsr',15,0,0,0,'0000-00-00',1,0,0,NULL,1,0,'sdrfsdr',1,0,0,'',0,1,0,0);
/*!40000 ALTER TABLE `products` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `providers`
--

DROP TABLE IF EXISTS `providers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `providers` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL DEFAULT '',
  `address` varchar(255) DEFAULT NULL,
  `phone` varchar(50) DEFAULT NULL,
  `cellphone` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`id`,`name`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `providers`
--

LOCK TABLES `providers` WRITE;
/*!40000 ALTER TABLE `providers` DISABLE KEYS */;
INSERT INTO `providers` VALUES (1,'No provider','-NA-','-NA-','-NA-');
/*!40000 ALTER TABLE `providers` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `random_msgs`
--

DROP TABLE IF EXISTS `random_msgs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `random_msgs` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `message` varchar(512) DEFAULT NULL,
  `season` int(10) unsigned NOT NULL DEFAULT '1',
  `count` bigint(20) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `random_msgs`
--

LOCK TABLES `random_msgs` WRITE;
/*!40000 ALTER TABLE `random_msgs` DISABLE KEYS */;
/*!40000 ALTER TABLE `random_msgs` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `reservations`
--

DROP TABLE IF EXISTS `reservations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `reservations` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `transaction_id` bigint(20) unsigned NOT NULL,
  `client_id` bigint(20) unsigned NOT NULL,
  `date` date NOT NULL DEFAULT '2010-01-01',
  `status` int(5) unsigned NOT NULL DEFAULT '1',
  `payment` double unsigned NOT NULL DEFAULT '0',
  `total` double unsigned NOT NULL DEFAULT '0',
  `totaltaxes` double unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `reservations`
--

LOCK TABLES `reservations` WRITE;
/*!40000 ALTER TABLE `reservations` DISABLE KEYS */;
/*!40000 ALTER TABLE `reservations` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `so_status`
--

DROP TABLE IF EXISTS `so_status`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `so_status` (
  `id` int(10) unsigned NOT NULL DEFAULT '0',
  `text` varchar(50) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `so_status`
--

LOCK TABLES `so_status` WRITE;
/*!40000 ALTER TABLE `so_status` DISABLE KEYS */;
INSERT INTO `so_status` VALUES (0,'Pending'),(1,'In Progress'),(2,'Ready'),(3,'Delivered'),(4,'Cancelled');
/*!40000 ALTER TABLE `so_status` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `special_orders`
--

DROP TABLE IF EXISTS `special_orders`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `special_orders` (
  `orderid` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL DEFAULT 'unknown',
  `groupElements` varchar(1000) DEFAULT '',
  `qty` double unsigned NOT NULL DEFAULT '1',
  `price` double unsigned NOT NULL DEFAULT '0',
  `cost` double unsigned NOT NULL DEFAULT '0',
  `units` int(10) unsigned NOT NULL DEFAULT '0',
  `status` int(10) DEFAULT '0',
  `saleid` bigint(20) unsigned NOT NULL DEFAULT '1',
  `notes` varchar(800) DEFAULT '',
  `payment` double unsigned NOT NULL DEFAULT '0',
  `completePayment` tinyint(1) DEFAULT '0',
  `dateTime` datetime NOT NULL DEFAULT '2009-01-01 00:00:00',
  `deliveryDateTime` datetime NOT NULL DEFAULT '2009-01-01 00:00:00',
  `clientId` bigint(20) unsigned NOT NULL DEFAULT '1',
  `userId` bigint(20) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`orderid`),
  KEY `SEC` (`saleid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `special_orders`
--

LOCK TABLES `special_orders` WRITE;
/*!40000 ALTER TABLE `special_orders` DISABLE KEYS */;
/*!40000 ALTER TABLE `special_orders` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `stock_corrections`
--

DROP TABLE IF EXISTS `stock_corrections`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `stock_corrections` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `product_id` bigint(20) unsigned NOT NULL,
  `new_stock_qty` bigint(20) unsigned NOT NULL,
  `old_stock_qty` bigint(20) unsigned NOT NULL,
  `reason` varchar(255) NOT NULL,
  `date` varchar(20) NOT NULL DEFAULT '2009-01-01',
  `time` varchar(20) NOT NULL DEFAULT '00:00',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `stock_corrections`
--

LOCK TABLES `stock_corrections` WRITE;
/*!40000 ALTER TABLE `stock_corrections` DISABLE KEYS */;
INSERT INTO `stock_corrections` VALUES (1,1111,100,0,'add','2012-06-15','04:43'),(2,4950232123566,200,0,'add','2012-06-15','04:43');
/*!40000 ALTER TABLE `stock_corrections` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `taxelements`
--

DROP TABLE IF EXISTS `taxelements`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `taxelements` (
  `elementid` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `ename` varchar(50) NOT NULL,
  `rate` double unsigned NOT NULL,
  PRIMARY KEY (`elementid`)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `taxelements`
--

LOCK TABLES `taxelements` WRITE;
/*!40000 ALTER TABLE `taxelements` DISABLE KEYS */;
INSERT INTO `taxelements` VALUES (1,'Exento de impuestos',0),(2,'IVA',16),(3,'Impuesto al tabaco',5),(4,'Impuesto a las comunicaciones',2);
/*!40000 ALTER TABLE `taxelements` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `taxmodels`
--

DROP TABLE IF EXISTS `taxmodels`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `taxmodels` (
  `modelid` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `tname` varchar(50) NOT NULL,
  `elementsid` varchar(50) NOT NULL,
  PRIMARY KEY (`modelid`)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `taxmodels`
--

LOCK TABLES `taxmodels` WRITE;
/*!40000 ALTER TABLE `taxmodels` DISABLE KEYS */;
INSERT INTO `taxmodels` VALUES (1,'Exento','1'),(2,'General','2'),(3,'Cigarros','2,3'),(4,'Comunicaciones','3,4');
/*!40000 ALTER TABLE `taxmodels` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `transactionitems`
--

DROP TABLE IF EXISTS `transactionitems`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `transactionitems` (
  `transaction_id` bigint(20) unsigned NOT NULL,
  `position` int(10) unsigned NOT NULL,
  `product_id` bigint(20) unsigned NOT NULL,
  `qty` double DEFAULT NULL,
  `points` double DEFAULT NULL,
  `unitstr` varchar(50) DEFAULT NULL,
  `cost` double DEFAULT NULL,
  `price` double DEFAULT NULL,
  `disc` double DEFAULT NULL,
  `total` double DEFAULT NULL,
  `name` varchar(255) DEFAULT NULL,
  `payment` double DEFAULT '0',
  `completePayment` tinyint(1) DEFAULT '0',
  `soId` varchar(255) DEFAULT '',
  `isGroup` tinyint(1) DEFAULT '0',
  `deliveryDateTime` datetime DEFAULT '2009-01-01 00:00:00',
  `tax` double DEFAULT '0',
  UNIQUE KEY `transaction_id` (`transaction_id`,`position`),
  KEY `product_id` (`product_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `transactionitems`
--

LOCK TABLES `transactionitems` WRITE;
/*!40000 ALTER TABLE `transactionitems` DISABLE KEYS */;
INSERT INTO `transactionitems` VALUES (3,1,4950232123566,1,10,'Pc',1,2,0,2,'Ciao',0,1,'',0,NULL,0),(20,1,1111,2,10,'Pc',12,15,0,30,'Ciao2',0,1,'',0,NULL,0),(20,2,2345,1,0,'Pc',3,3.03,0,3.03,'sdfsdf',0,1,'',0,NULL,1),(22,1,2345,1,0,'Pc',3,3.03,0,3.03,'sdfsdf',0,1,'',0,NULL,1),(25,1,1111,1,10,'Pc',12,15,0,15,'Ciao2',0,1,'',0,NULL,0),(25,2,2345,1,0,'Pc',3,3.03,0,3.03,'sdfsdf',0,1,'',0,NULL,1),(38,1,4950232123566,2,10,'Pc',1,2,0,4,'Ciao',0,1,'',0,NULL,0),(48,1,1111,4,10,'Pc',12,15,0,60,'Ciao2',0,1,'',0,NULL,0),(50,1,1111,3,10,'Pc',12,15,0,45,'Ciao2',0,1,'',0,NULL,0);
/*!40000 ALTER TABLE `transactionitems` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `transactions`
--

DROP TABLE IF EXISTS `transactions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `transactions` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `clientid` int(10) unsigned NOT NULL,
  `type` smallint(5) unsigned DEFAULT NULL,
  `amount` double unsigned NOT NULL DEFAULT '0',
  `date` date NOT NULL DEFAULT '2009-01-01',
  `time` time NOT NULL DEFAULT '00:00:00',
  `paidwith` double unsigned NOT NULL DEFAULT '0',
  `changegiven` double unsigned NOT NULL DEFAULT '0',
  `paymethod` int(10) NOT NULL DEFAULT '0',
  `state` int(10) NOT NULL DEFAULT '0',
  `userid` int(10) NOT NULL DEFAULT '0',
  `cardnumber` varchar(20) DEFAULT NULL,
  `itemcount` int(10) unsigned NOT NULL DEFAULT '0',
  `itemslist` varchar(1000) NOT NULL,
  `points` bigint(20) unsigned NOT NULL DEFAULT '0',
  `discmoney` double NOT NULL DEFAULT '0',
  `disc` double NOT NULL DEFAULT '0',
  `cardauthnumber` varchar(50) NOT NULL,
  `utility` double NOT NULL DEFAULT '0',
  `terminalnum` int(10) unsigned NOT NULL DEFAULT '1',
  `providerid` int(10) unsigned NOT NULL DEFAULT '1',
  `specialOrders` varchar(1000) DEFAULT '',
  `balanceId` bigint(20) unsigned NOT NULL DEFAULT '1',
  `totalTax` double NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `SEC` (`clientid`,`type`,`date`,`time`,`state`)
) ENGINE=MyISAM AUTO_INCREMENT=68 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `transactions`
--

LOCK TABLES `transactions` WRITE;
/*!40000 ALTER TABLE `transactions` DISABLE KEYS */;
INSERT INTO `transactions` VALUES (15,1,2,24,'2012-05-14','22:04:00',0,0,1,2,1,'-NA-',2,'1111/2',0,0,0,'-NA-',0,0,1,'',0,0),(3,1,1,2,'2012-01-23','22:35:00',2,0,1,2,1,'',1,'4950232123566/1',10,0,0,'',1,1,1,NULL,6,0),(13,1,2,240,'2012-04-16','22:51:00',0,0,1,2,1,'-NA-',80,'2345/80',0,0,0,'-NA-',0,0,1,'',38100608,0),(12,1,2,15,'2012-04-16','22:50:00',0,0,1,2,1,'-NA-',5,'2345/5',0,0,0,'-NA-',0,0,1,'',38100608,0),(48,2,1,60,'2012-07-18','15:40:00',60,0,3,8,2,'',4,'1111/4',280,0,0,'',12,1,1,NULL,37,0),(20,1,1,33,'2012-06-15','04:39:00',33,0,3,8,2,'',3,'1111/2,2345/1',470,0,0,'',6.029999999999999,1,1,NULL,13,0.03),(22,1,1,3,'2012-06-15','04:42:00',3,0,3,8,2,'',1,'2345/1',0,0,0,'',0.029999999999999805,1,1,NULL,14,0.03),(38,2,1,4,'2012-06-16','01:07:00',4,0,3,8,2,'',2,'4950232123566/2',110,0,0,'',2,1,1,NULL,28,0),(25,2,1,18,'2012-06-15','04:46:00',18,0,3,8,2,'',2,'1111/1,2345/1',90,0,0,'',3.03,1,1,NULL,16,0.03),(50,2,1,45,'2012-07-18','15:40:00',45,0,3,8,2,'',3,'1111/3',210,0,0,'',9,1,1,NULL,38,0),(61,1,2,1200,'2012-07-23','20:45:00',0,0,1,2,1,'-NA-',100,'1111/100',140509130904850,1.9095076e-316,6.94207340070734e-310,'-NA-',0,0,1,'',0,0);
/*!40000 ALTER TABLE `transactions` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `transactionstates`
--

DROP TABLE IF EXISTS `transactionstates`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `transactionstates` (
  `stateid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `text` varchar(50) NOT NULL,
  PRIMARY KEY (`stateid`)
) ENGINE=MyISAM AUTO_INCREMENT=8 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `transactionstates`
--

LOCK TABLES `transactionstates` WRITE;
/*!40000 ALTER TABLE `transactionstates` DISABLE KEYS */;
INSERT INTO `transactionstates` VALUES (1,'Not Completed'),(2,'Completed'),(3,'Cancelled'),(4,'PO Pending'),(5,'PO Completed'),(6,'PO Incomplete'),(7,'Reservation');
/*!40000 ALTER TABLE `transactionstates` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `transactiontypes`
--

DROP TABLE IF EXISTS `transactiontypes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `transactiontypes` (
  `ttypeid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `text` varchar(50) NOT NULL,
  PRIMARY KEY (`ttypeid`)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `transactiontypes`
--

LOCK TABLES `transactiontypes` WRITE;
/*!40000 ALTER TABLE `transactiontypes` DISABLE KEYS */;
INSERT INTO `transactiontypes` VALUES (1,'Sell'),(2,'Purchase'),(3,'Change'),(4,'Return');
/*!40000 ALTER TABLE `transactiontypes` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `users`
--

DROP TABLE IF EXISTS `users`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `users` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `username` varchar(50) NOT NULL DEFAULT '',
  `password` varchar(50) DEFAULT NULL,
  `salt` varchar(5) DEFAULT NULL,
  `name` varchar(255) DEFAULT '',
  `address` varchar(255) DEFAULT NULL,
  `phone` varchar(50) DEFAULT NULL,
  `phone_movil` varchar(50) DEFAULT NULL,
  `role` int(10) unsigned DEFAULT '0',
  `photo` blob,
  PRIMARY KEY (`id`),
  KEY `SEC` (`username`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `users`
--

LOCK TABLES `users` WRITE;
/*!40000 ALTER TABLE `users` DISABLE KEYS */;
INSERT INTO `users` VALUES (1,'admin','','h60VK','Administrator',NULL,NULL,NULL,2,NULL),(2,'daniele','0E511F5A42903F6DD1E9F54E6D2D923729FBAC06','_z~uW','Daniele Paganelli','','','',2,NULL);
/*!40000 ALTER TABLE `users` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Temporary table structure for view `v_groupedSO`
--

DROP TABLE IF EXISTS `v_groupedSO`;
/*!50001 DROP VIEW IF EXISTS `v_groupedSO`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `v_groupedSO` (
  `orderid` bigint(20) unsigned,
  `name` varchar(255),
  `groupElements` varchar(1000),
  `qty` double unsigned,
  `price` double unsigned,
  `cost` double unsigned,
  `units` int(10) unsigned,
  `status` int(10),
  `saleid` bigint(20) unsigned,
  `notes` varchar(800),
  `payment` double unsigned,
  `completePayment` tinyint(1),
  `dateTime` datetime,
  `deliveryDateTime` datetime,
  `clientId` bigint(20) unsigned,
  `userId` bigint(20) unsigned
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `v_transS`
--

DROP TABLE IF EXISTS `v_transS`;
/*!50001 DROP VIEW IF EXISTS `v_transS`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `v_transS` (
  `id` bigint(20) unsigned,
  `userid` int(10),
  `clientid` int(10) unsigned,
  `date` date,
  `time` time,
  `state` int(10),
  `itemslist` varchar(1000),
  `terminalnum` int(10) unsigned,
  `itemcount` int(10) unsigned
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `v_transactionitems`
--

DROP TABLE IF EXISTS `v_transactionitems`;
/*!50001 DROP VIEW IF EXISTS `v_transactionitems`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `v_transactionitems` (
  `datetime` varchar(21),
  `id` bigint(20) unsigned,
  `points` double,
  `name` varchar(255),
  `price` double,
  `disc` double,
  `total` double,
  `clientid` int(10) unsigned,
  `userid` int(10),
  `date` date,
  `time` time,
  `position` int(10) unsigned,
  `product_id` bigint(20) unsigned,
  `cost` double
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `v_transactions`
--

DROP TABLE IF EXISTS `v_transactions`;
/*!50001 DROP VIEW IF EXISTS `v_transactions`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `v_transactions` (
  `datetime` varchar(21),
  `id` bigint(20) unsigned,
  `clientid` int(10) unsigned,
  `userid` int(10),
  `itemcount` int(10) unsigned,
  `disc` double,
  `amount` double unsigned,
  `date` date
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `v_transactionsbydate`
--

DROP TABLE IF EXISTS `v_transactionsbydate`;
/*!50001 DROP VIEW IF EXISTS `v_transactionsbydate`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `v_transactionsbydate` (
  `date` date,
  `transactions` bigint(21),
  `items` decimal(32,0),
  `total` double
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Final view structure for view `v_groupedSO`
--

/*!50001 DROP TABLE IF EXISTS `v_groupedSO`*/;
/*!50001 DROP VIEW IF EXISTS `v_groupedSO`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = latin1 */;
/*!50001 SET character_set_results     = latin1 */;
/*!50001 SET collation_connection      = latin1_swedish_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `v_groupedSO` AS select `special_orders`.`orderid` AS `orderid`,`special_orders`.`name` AS `name`,`special_orders`.`groupElements` AS `groupElements`,`special_orders`.`qty` AS `qty`,`special_orders`.`price` AS `price`,`special_orders`.`cost` AS `cost`,`special_orders`.`units` AS `units`,`special_orders`.`status` AS `status`,`special_orders`.`saleid` AS `saleid`,`special_orders`.`notes` AS `notes`,`special_orders`.`payment` AS `payment`,`special_orders`.`completePayment` AS `completePayment`,`special_orders`.`dateTime` AS `dateTime`,`special_orders`.`deliveryDateTime` AS `deliveryDateTime`,`special_orders`.`clientId` AS `clientId`,`special_orders`.`userId` AS `userId` from `special_orders` group by `special_orders`.`saleid` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `v_transS`
--

/*!50001 DROP TABLE IF EXISTS `v_transS`*/;
/*!50001 DROP VIEW IF EXISTS `v_transS`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = latin1 */;
/*!50001 SET character_set_results     = latin1 */;
/*!50001 SET collation_connection      = latin1_swedish_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `v_transS` AS select `transactions`.`id` AS `id`,`transactions`.`userid` AS `userid`,`transactions`.`clientid` AS `clientid`,`transactions`.`date` AS `date`,`transactions`.`time` AS `time`,`transactions`.`state` AS `state`,`transactions`.`itemslist` AS `itemslist`,`transactions`.`terminalnum` AS `terminalnum`,`transactions`.`itemcount` AS `itemcount` from `transactions` where ((`transactions`.`state` = 1) and (`transactions`.`type` = 1) and (`transactions`.`itemcount` > 0)) order by `transactions`.`id` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `v_transactionitems`
--

/*!50001 DROP TABLE IF EXISTS `v_transactionitems`*/;
/*!50001 DROP VIEW IF EXISTS `v_transactionitems`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = latin1 */;
/*!50001 SET character_set_results     = latin1 */;
/*!50001 SET collation_connection      = latin1_swedish_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `v_transactionitems` AS select concat(date_format(`t`.`date`,'%d/%m/%Y'),' ',time_format(`t`.`time`,'%H:%i')) AS `datetime`,`t`.`id` AS `id`,`ti`.`points` AS `points`,`ti`.`name` AS `name`,`ti`.`price` AS `price`,`ti`.`disc` AS `disc`,`ti`.`total` AS `total`,`t`.`clientid` AS `clientid`,`t`.`userid` AS `userid`,`t`.`date` AS `date`,`t`.`time` AS `time`,`ti`.`position` AS `position`,`ti`.`product_id` AS `product_id`,`ti`.`cost` AS `cost` from (`transactions` `t` join `transactionitems` `ti`) where ((`t`.`id` = `ti`.`transaction_id`) and (`t`.`type` = 1) and (`t`.`state` = 2)) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `v_transactions`
--

/*!50001 DROP TABLE IF EXISTS `v_transactions`*/;
/*!50001 DROP VIEW IF EXISTS `v_transactions`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = latin1 */;
/*!50001 SET character_set_results     = latin1 */;
/*!50001 SET collation_connection      = latin1_swedish_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `v_transactions` AS select concat(date_format(`t`.`date`,'%d/%m/%Y'),' ',time_format(`t`.`time`,'%H:%i')) AS `datetime`,`t`.`id` AS `id`,`t`.`clientid` AS `clientid`,`t`.`userid` AS `userid`,`t`.`itemcount` AS `itemcount`,`t`.`disc` AS `disc`,`t`.`amount` AS `amount`,`t`.`date` AS `date` from `transactions` `t` where ((`t`.`type` = 1) and (`t`.`state` = 2)) order by concat(date_format(`t`.`date`,'%d/%m/%Y'),' ',time_format(`t`.`time`,'%H:%i')) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `v_transactionsbydate`
--

/*!50001 DROP TABLE IF EXISTS `v_transactionsbydate`*/;
/*!50001 DROP VIEW IF EXISTS `v_transactionsbydate`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = latin1 */;
/*!50001 SET character_set_results     = latin1 */;
/*!50001 SET collation_connection      = latin1_swedish_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `v_transactionsbydate` AS select `transactions`.`date` AS `date`,count(1) AS `transactions`,sum(`transactions`.`itemcount`) AS `items`,sum(`transactions`.`amount`) AS `total` from `transactions` where ((`transactions`.`type` = 1) and (`transactions`.`itemcount` > 0) and (`transactions`.`state` = 2)) group by `transactions`.`date` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2012-10-29  9:12:09
