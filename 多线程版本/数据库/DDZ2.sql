/*
Navicat MySQL Data Transfer

Source Server         : localhost_3306
Source Server Version : 50537
Source Host           : localhost:3306
Source Database       : DDZ2

Target Server Type    : MYSQL
Target Server Version : 50537
File Encoding         : 65001

Date: 2014-09-19 15:57:56
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for game
-- ----------------------------
DROP TABLE IF EXISTS `game`;
CREATE TABLE `game` (
  `gameID` int(11) NOT NULL DEFAULT '0',
  `landlord` varchar(20) NOT NULL,
  PRIMARY KEY (`gameID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='utf8_general_ci';

-- ----------------------------
-- Records of game
-- ----------------------------
INSERT INTO `game` VALUES ('1001', 'lizhuoli');

-- ----------------------------
-- Table structure for users
-- ----------------------------
DROP TABLE IF EXISTS `users`;
CREATE TABLE `users` (
  `username` varchar(20) NOT NULL DEFAULT '',
  `password` varchar(40) NOT NULL,
  `status` enum('gaming','ready','online','offline') NOT NULL DEFAULT 'offline',
  `gameID` int(10) unsigned NOT NULL,
  `score` int(11) NOT NULL DEFAULT '0',
  `readytime` time NOT NULL DEFAULT '00:00:00',
  PRIMARY KEY (`username`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='utf8_general_ci';

-- ----------------------------
-- Records of users
-- ----------------------------
INSERT INTO `users` VALUES ('lizhuoli', '941126', 'ready', '0', '0', '15:43:04');
INSERT INTO `users` VALUES ('lizhuoli1', '941126', 'ready', '0', '0', '15:43:10');
INSERT INTO `users` VALUES ('lizhuoli2', '941126', 'ready', '0', '0', '15:43:14');
INSERT INTO `users` VALUES ('lizhuoli3', '941126', 'offline', '0', '0', '00:00:00');
