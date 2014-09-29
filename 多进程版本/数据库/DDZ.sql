/*
Navicat MySQL Data Transfer

Source Server         : localhost_3306
Source Server Version : 50537
Source Host           : localhost:3306
Source Database       : DDZ

Target Server Type    : MYSQL
Target Server Version : 50537
File Encoding         : 65001

Date: 2014-09-15 00:32:42
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for users
-- ----------------------------
DROP TABLE IF EXISTS `users`;
CREATE TABLE `users` (
  `username` varchar(20) NOT NULL DEFAULT '',
  `password` varchar(40) NOT NULL,
  `status` enum('gaming','ready','online','offline') NOT NULL DEFAULT 'offline',
  PRIMARY KEY (`username`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='utf8_general_ci';

-- ----------------------------
-- Records of users
-- ----------------------------
INSERT INTO `users` VALUES ('lizhuoli2', '941126', 'offline');
INSERT INTO `users` VALUES ('lizhuoli1', '941126', 'online');
INSERT INTO `users` VALUES ('lizhuoli3', '941126', 'offline');
INSERT INTO `users` VALUES ('lizhuoli', '941126', 'offline');
INSERT INTO `users` VALUES ('lizhuoli4', '941126', 'offline');
