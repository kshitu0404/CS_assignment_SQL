CREATE DATABASE IF NOT EXISTS sql_injection_lab;
USE sql_injection_lab;

CREATE TABLE IF NOT EXISTS users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) NOT NULL UNIQUE,
    password VARCHAR(100) NOT NULL
);

INSERT IGNORE INTO users (username, password)
VALUES ('admin', 'admin123');

