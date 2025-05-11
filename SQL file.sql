-- Create the database
CREATE DATABASE voting_system;
USE voting_system;

-- Table for users (admin and voters)
CREATE TABLE Users (
    username VARCHAR(50) PRIMARY KEY,
    password VARCHAR(50) NOT NULL,
    role ENUM('admin') NOT NULL,
    city VARCHAR(50)  -- Only for voters, admin can have empty city
);
-- select * from Users;

CREATE TABLE Voters (
    id INT AUTO_INCREMENT UNIQUE,
    username VARCHAR(50) PRIMARY KEY,  
    password VARCHAR(50) NOT NULL,
    role ENUM('voter') NOT NULL,
    city VARCHAR(50) NOT NULL,
    MNA_vote BOOLEAN DEFAULT 0,
    MPA_vote BOOLEAN DEFAULT 0,
    vote_status BOOLEAN DEFAULT 0
);

select * from Voters;


DELIMITER //
CREATE TRIGGER set_usernamepassVoter_before_insert
BEFORE INSERT ON Voters
FOR EACH ROW
BEGIN
    DECLARE next_id INT;
    SELECT AUTO_INCREMENT INTO next_id
    FROM information_schema.TABLES
    WHERE TABLE_NAME = 'Voters' AND TABLE_SCHEMA = DATABASE();

    IF NEW.username IS NULL OR NEW.username = '' THEN
        SET NEW.username = CONCAT('voter', next_id);
    END IF;

    IF NEW.password IS NULL OR NEW.password = '' THEN
        SET NEW.password = CONCAT('pass', next_id);
    END IF;
END;
//
DELIMITER ;




-- drop trigger set_usernamepassVoter_after_insert;
    
-- UPDATED at 7:25 PM on 10 MAY

-- CANDIDATES

CREATE TABLE Candidates (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) NOT NULL,
    password VARCHAR(50) NOT NULL unique,
    party ENUM ('PTI', 'PMLN') NOT NULL,
    city VARCHAR(50) NOT NULL,
    votes INT DEFAULT 0,
    title ENUM ('MPA', 'MNA', 'Chairman', 'Secretary') NOT NULL
);

DELIMITER //
CREATE TRIGGER set_usernamepass_before_insert
BEFORE INSERT ON Candidates
FOR EACH ROW
BEGIN
    -- Get the next auto-increment value
    DECLARE next_id INT;
    
    SELECT AUTO_INCREMENT INTO next_id
    FROM information_schema.TABLES
    WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'Candidates';
    
    -- Set username and password based on the next ID
    SET NEW.username = CONCAT('candidate', next_id);
    SET NEW.password = CONCAT('pass', next_id);
END//
DELIMITER ;

select * from Candidates;

-- Table for votes
CREATE TABLE Votes (
	id int unique auto_increment,
    username VARCHAR(50),
    candidate_id INT,
    vote_type ENUM('MNA', 'MPA') NOT NULL,
    PRIMARY KEY (username, vote_type),
    FOREIGN KEY (username) REFERENCES Voters(username),
    FOREIGN KEY (candidate_id) REFERENCES Candidates(id)
);

select * from votes;


-- Table for controlling voting status
CREATE TABLE VotingStatus (
    id INT PRIMARY KEY,
    is_open BOOLEAN
);
-- select * from VotingStatus;

create table Province (
	p_id int PRIMARY KEY auto_increment,
	name varchar(50) NOT NULL UNIQUE
    );

-- select * from Province;

create table Cities (
	id int Primary key auto_increment,
    name varchar(50) NOT NULL UNIQUE, 
    p_id int NOT NULL,
	foreign	key (p_id) references Province(p_id)
    );
    
 select * from Cities;


-- Insert initial voting status (closed by default)
INSERT INTO VotingStatus (id, is_open) VALUES (1, 1);


-- INSERTING DATA INTO USERS
INSERT INTO Users (username, password, role, city) VALUES
('admin', 'admin123', 'admin', '');

-- INSERTING DATA INTO VOTERS 
INSERT INTO Voters (username, password, role, city, MNA_vote, MPA_vote, Vote_status) VALUES
('voter1', 'pass1', 'voter', 'Lahore', 0, 0, 0),
('voter2', 'pass2', 'voter', 'Karachi', 0, 0, 0),
('voter3', 'pass3', 'voter', 'Peshawar', 0, 0, 0),
('voter4', 'pass4', 'voter', 'Quetta', 0, 0, 0),
('voter5', 'pass5', 'voter', 'Gilgit', 0, 0, 0),
('voter6','pass6','voter','Islamabad', 0, 0, 0);


-- INSERTING DATA INTO CANDIDATES
INSERT INTO Candidates (username, password, party, city, title) VALUES

-- PUNJAB
('candidate1', 'pass1', 'PTI', 'lahore', 'MNA'),
('candidate2', 'pass2', 'PTI', 'lahore', 'MPA'),
('candidate3', 'pass3', 'PMLN', 'lahore', 'MNA'),
('candidate4', 'pass4', 'PMLN', 'lahore', 'MPA'),
('candidate5', 'pass5', 'PTI', 'sialkot', 'MNA'),
('candidate6', 'pass6', 'PTI', 'sialkot', 'MPA'),
('candidate7', 'pass7', 'PMLN', 'sialkot', 'MNA'),
('candidate8', 'pass8', 'PMLN', 'sialkot', 'MPA'),
('candidate9', 'pass9', 'PTI', 'multan', 'MNA'),
('candidate10', 'pass10', 'PTI', 'multan', 'MPA'),
('candidate11', 'pass11', 'PMLN', 'multan', 'MNA'),
('candidate12', 'pass12', 'PMLN', 'multan', 'MPA'),
('candidate13', 'pass13', 'PTI', 'gujranwala', 'MNA'),
('candidate14', 'pass14', 'PTI', 'gujranwala', 'MPA'),
('candidate15', 'pass15', 'PMLN', 'gujranwala', 'MNA'),
('candidate16', 'pass16', 'PMLN', 'gujranwala', 'MPA'),
('candidate17', 'pass17', 'PTI', 'faisalabad', 'MNA'),
('candidate18', 'pass18', 'PTI', 'faisalabad', 'MPA'),
('candidate19', 'pass19', 'PMLN', 'faisalabad', 'MNA'),
('candidate20', 'pass20', 'PMLN', 'faisalabad', 'MPA'),

-- SINDH
('candidate21', 'pass21', 'PTI', 'karachi', 'MNA'),
('candidate22', 'pass22', 'PTI', 'karachi', 'MPA'),
('candidate23', 'pass23', 'PMLN', 'karachi', 'MNA'),
('candidate24', 'pass24', 'PMLN', 'karachi', 'MPA'),
('candidate25', 'pass25', 'PTI', 'hyderabad', 'MNA'),
('candidate26', 'pass26', 'PTI', 'hyderabad', 'MPA'),
('candidate27', 'pass27', 'PMLN', 'hyderabad', 'MNA'),
('candidate28', 'pass28', 'PMLN', 'hyderabad', 'MPA'),
('candidate29', 'pass29', 'PTI', 'jacobabad', 'MNA'),
('candidate30', 'pass30', 'PTI', 'jacobabad', 'MPA'),
('candidate31', 'pass31', 'PMLN', 'jacobabad', 'MNA'),
('candidate32', 'pass32', 'PMLN', 'jacobabad', 'MPA'),
('candidate33', 'pass33', 'PTI', 'larkana', 'MNA'),
('candidate34', 'pass34', 'PTI', 'larkana', 'MPA'),
('candidate35', 'pass35', 'PMLN', 'larkana', 'MNA'),
('candidate36', 'pass36', 'PMLN', 'larkana', 'MPA'),
('candidate37', 'pass37', 'PTI', 'khairpur', 'MNA'),
('candidate38', 'pass38', 'PTI', 'khaipur', 'MPA'),
('candidate39', 'pass39', 'PMLN', 'khairpur', 'MNA'),
('candidate40', 'pass40', 'PMLN', 'khaipur', 'MPA'),

-- BALOCHISTAN
('candidate41', 'pass41', 'PTI', 'quetta', 'MNA'),
('candidate42', 'pass42', 'PTI', 'quetta', 'MPA'),
('candidate43', 'pass43', 'PMLN', 'quetta', 'MNA'),
('candidate44', 'pass44', 'PMLN', 'quetta', 'MPA'),
('candidate45', 'pass45', 'PTI', 'gawadar', 'MNA'),
('candidate46', 'pass46', 'PTI', 'gawadar', 'MPA'),
('candidate47', 'pass47', 'PMLN', 'gawadar', 'MNA'),
('candidate48', 'pass48', 'PMLN', 'gawadar', 'MPA'),
('candidate49', 'pass49', 'PTI', 'sibi', 'MNA'),
('candidate50', 'pass50', 'PTI', 'sibi', 'MPA'),
('candidate51', 'pass51', 'PMLN', 'sibi', 'MNA'),
('candidate52', 'pass52', 'PMLN', 'sibi', 'MPA'),
('candidate53', 'pass53', 'PTI', 'kalat', 'MNA'),
('candidate54', 'pass54', 'PTI', 'kalat', 'MPA'),
('candidate55', 'pass55', 'PMLN', 'kalat', 'MNA'),
('candidate56', 'pass56', 'PMLN', 'kalat', 'MPA'),
('candidate57', 'pass57', 'PTI', 'chaman', 'MNA'),
('candidate58', 'pass58', 'PTI', 'chaman', 'MPA'),
('candidate59', 'pass59', 'PMLN', 'chaman', 'MNA'),
('candidate60', 'pass60', 'PMLN', 'chaman', 'MPA'),

-- KPK
('candidate61', 'pass61', 'PTI', 'peshawar', 'MNA'),
('candidate62', 'pass62', 'PTI', 'peshawar', 'MPA'),
('candidate63', 'pass63', 'PMLN', 'peshawar', 'MNA'),
('candidate64', 'pass64', 'PMLN', 'peshawar', 'MPA'),
('candidate65', 'pass65', 'PTI', 'kohat', 'MNA'),
('candidate66', 'pass66', 'PTI', 'kohat', 'MPA'),
('candidate67', 'pass67', 'PMLN', 'kohat', 'MNA'),
('candidate68', 'pass68', 'PMLN', 'kohat', 'MPA'),
('candidate69', 'pass69', 'PTI', 'bannu', 'MNA'),
('candidate70', 'pass70', 'PTI', 'bannu', 'MPA'),
('candidate71', 'pass71', 'PMLN', 'bannu', 'MNA'),
('candidate72', 'pass72', 'PMLN', 'bannu', 'MPA'),
('candidate73', 'pass73', 'PTI', 'mardan', 'MNA'),
('candidate74', 'pass74', 'PTI', 'mardan', 'MPA'),
('candidate75', 'pass75', 'PMLN', 'mardan', 'MNA'),
('candidate76', 'pass76', 'PMLN', 'mardan', 'MPA'),
('candidate77', 'pass77', 'PTI', 'mansehra', 'MNA'),
('candidate78', 'pass78', 'PTI', 'mansehra', 'MPA'),
('candidate79', 'pass79', 'PMLN', 'mansehra', 'MNA'),
('candidate80', 'pass80', 'PMLN', 'mansehra', 'MPA');

INSERT INTO Province (name) VALUES 
('punjab'),
('sindh'),
('balochistan'),
('kpk');

INSERT INTO Cities (name, p_id) VALUES 
('lahore', 1),
('sialkot', 1),
('multan', 1),
('gujranwala', 1),
('faisalabad', 1),
('karachi', 2),
('hyderabad', 2),
('jacobabad', 2),
('larkana', 2),
('khairpur', 2),
('quetta', 3),
('gawadar', 3),
('sibi', 3),
('kalat', 3),
('chaman', 3),
('peshawar', 4),
('kohat', 4),
('bannu', 4),
('mardan', 4),
('mansehra', 4);


drop database voting_system;