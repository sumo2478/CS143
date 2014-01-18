-- Define which database should be used
USE CS143; -- TODO: Change to CS143

-- Load file from movie.del
CREATE TABLE Movie
(
	id int PRIMARY KEY, -- Movie ID
	title varchar(100), -- Movie title
	year int,           -- Release year
	rating varchar(10), -- MPAA rating
	company varchar(50) -- Production Company
);

-- Load files from actor1.del, actor2.del, actor3.del
CREATE TABLE Actor 
(
	id int PRIMARY KEY, -- Actor ID
	last varchar(20),   -- Last name
	first varchar(20),  -- First name
	sex varchar(6),     -- Sex of actor
	dob DATE,           -- Date of birth
	dod DATE           -- Date of death
);

-- -- Load file is from director.del
CREATE TABLE Director
(
	id int PRIMARY KEY, -- Director ID (should be matched to actor)
	last varchar(20),   -- Last name
	first varchar(20),  -- First name
	dob DATE,           -- Date of birth
	dod DATE           -- Date of death
);

-- Load file is from moviegenre.del
CREATE TABLE MovieGenre
(
	mid int,           -- Movie ID
	genre varchar(20) -- Movie Genre
);

-- Load file is from moviedirector.del
CREATE TABLE MovieDirector
(
	mid int -- Movie ID
);

-- Load file is from movieactor1.del and movieactor2.del
CREATE TABLE MovieActor
(
	mid int,         -- Movie ID
	aid int, 		 -- Actor ID
	role varchar(50) -- Actor role in movie
);

CREATE TABLE Review
(
	name varchar(20),    -- Reviewer name
	time TIMESTAMP,      -- Review time
	mid int,             -- Movie ID
	rating int,          -- Review rating
	comment varchar(500) -- Reviewer comment
);

CREATE TABLE MaxPersonID
(
	id int -- Max ID assigned to all persons
);

CREATE TABLE MaxMovieId
(
	id int -- Max ID assigned to all movies
);

-- Insert tuple to MaxPersonID
INSERT INTO MaxPersonID values (69000);

-- Insert tuple to MaxMovieId
INSERT INTO MaxMovieId values (4750);

