-- Define which database should be used
-- USE CS143; -- TODO: Change to CS143

-- Load file from movie.del
CREATE TABLE Movie
(
	id INT, -- Movie ID
	title VARCHAR(100) NOT NULL, -- Movie title
	year INT,                    -- Release year
	rating VARCHAR(10), 		 -- MPAA rating
	company VARCHAR(50), 		 -- Production Company
	PRIMARY KEY(id)              -- Movie id should be unique
) ENGINE = INNODB;

-- Load files from actor1.del, actor2.del, actor3.del
CREATE TABLE Actor 
(
	id INT, -- Actor ID
	last VARCHAR(20),   -- Last name
	first VARCHAR(20),  -- First name
	sex VARCHAR(6),     -- Sex of actor
	dob DATE,           -- Date of birth
	dod DATE,           -- Date of death
	PRIMARY KEY(id),    -- Actor id should be unique
	CHECK(dob < dod),   -- We check to make sure the Actor's date of birth is less than his/her date of death
	CHECK(sex = "Male" OR sex = "Female") -- We check to make sure the Actor's sex is either a Male or Female
) ENGINE = INNODB;

-- -- Load file is from director.del
CREATE TABLE Director
(
	id int, -- Director ID (should be matched to actor)
	last varchar(20),   -- Last name
	first varchar(20),  -- First name
	dob DATE,           -- Date of birth
	dod DATE,           -- Date of death
	PRIMARY KEY(id),    -- Director id should be unique
	CHECK (dob < dod)   -- We check to make sure the Director's date of birth is less than his/her date of death
) ENGINE = INNODB;

-- Load file is from moviegenre.del
CREATE TABLE MovieGenre
(
	mid int,           -- Movie ID
	genre varchar(20), -- Movie Genre
	FOREIGN KEY(mid) references Movie(id) -- We check to make sure the movie id exists in the Movie table
) ENGINE = INNODB;

-- Load file is from moviedirector.del
CREATE TABLE MovieDirector
(
	mid INT, -- Movie ID
	did INT,  -- Director ID
	FOREIGN KEY (mid) references Movie(id),  -- We check to make sure the movie id exists in the Movie table
	FOREIGN KEY (did) references Director(id) -- We check to make sure the director id exists in the Director table
) ENGINE = INNODB;

-- Load file is from movieactor1.del and movieactor2.del
CREATE TABLE MovieActor
(
	mid int,         -- Movie ID
	aid int, 		 -- Actor ID
	role varchar(50), -- Actor role in movie
	FOREIGN KEY (mid) references Movie(id), -- We check to make sure the movie id exists in the Movie table
	FOREIGN KEY (aid) references Actor(id)  -- We check to make sure the actor id exists in the Actor table
) ENGINE = INNODB;

CREATE TABLE Review
(
	name varchar(20),    -- Reviewer name
	time TIMESTAMP,      -- Review time
	mid int,             -- Movie ID
	rating int,          -- Review rating
	comment varchar(500), -- Reviewer comment
	FOREIGN KEY (mid) references Movie(id) -- We check to make sure the movie id exists in the Movie table
) ENGINE = INNODB;

CREATE TABLE MaxPersonID
(
	id int -- Max ID assigned to all persons
) ENGINE = INNODB;

CREATE TABLE MaxMovieID
(
	id int -- Max ID assigned to all movies
) ENGINE = INNODB;

-- Insert tuple to MaxPersonID
INSERT INTO MaxPersonID values (69000);

-- Insert tuple to MaxMovieId
INSERT INTO MaxMovieID values (4750);

