-- Three primary key constraints
--------------------------------

-- Movie id constraint
-- This modification violates the PRIMARY KEY definition for movie id
-- because there is already an existing movie with an id 2
-- Output: ERROR 1062 (23000): Duplicate entry '2' for key 1
INSERT INTO Movie VALUES (2, "Title", 1993, "PG-13", "Collin inc.");

-- Actor id constraint
-- This modification violates the PRIMARY KEY definition for actor id
-- because there is already an existing actor with an id 68635
-- Output: ERROR 1062 (23000): Duplicate entry '68635' for key 1
INSERT INTO Actor VALUES (68635, "Last", "First", "Male", "1990-10-04", "1990-11-04");

-- Director id constraint
-- This modification violates the PRIMARY KEY definition for director id
-- because there is already an existing director with id 68626
-- Output: ERROR 1062 (23000): Duplicate entry '68626' for key 1
INSERT INTO Director VALUES (68626, "Last", "First", "1990-10-04", "1990-12-03");

-- Six referential integrity constraints
----------------------------------------

-- MovieGenre - mid has a foreign key related to Movie.id
-- This modification violates the foreign key constraint because the movie id 999999 doesn't
-- exist within the Movie table
-- Output: ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint 
-- fails (`CS143/MovieGenre`, CONSTRAINT `MovieGenre_ibfk_1` FOREIGN KEY (`mid`) REFERENCES 
--`Movie` (`id`))
INSERT INTO MovieGenre values (999999, "Action");

-- MovieDirector - mid has a forein key related to Movie.id
-- This modification violates the foreign key constraint for the movie id 
-- because the movie doesn't exist in the movie table
-- Output: ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails (`CS143/MovieDirector`, CONSTRAINT `MovieDirector_ibfk_1` FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))
INSERT INTO MovieDirector values (999999, 68398);

-- MovieDirector - did has a foreign key related to Director.id
-- This modification violates the foreign key constraint for the director id
-- because the director doesn't exist in the Director table
-- Output: ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails (`CS143/MovieDirector`, CONSTRAINT `MovieDirector_ibfk_2` FOREIGN KEY (`did`) REFERENCES `Director` (`id`))
INSERT INTO MovieDirector values (4734, 999999);

-- MovieActor - mid has a foreign key related to Movie.id
-- This modification violates the foreign key constraint for the movie id 
-- because the movie doesn't exist in the Movie Table
-- Output: ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails (`CS143/MovieActor`, CONSTRAINT `MovieActor_ibfk_1` FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))
INSERT INTO MovieActor values (999999, 68635, "Main");

-- MovieActor - aid has a foreign key related to Actor.id
-- This modification violates the foreign key constraint for the actor id
-- because the actor doesn't exist in the Actor Table
-- Output: ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails (`CS143/MovieActor`, CONSTRAINT `MovieActor_ibfk_2` FOREIGN KEY (`aid`) REFERENCES `Actor` (`id`))
INSERT INTO MovieActor values (4734, 999999, "Main");

-- Review - mid has a foreign key related to Movie.id
-- This modification violates the foreign key constraint for the movie id
-- because the movie doesn't exist in the Movie Table
-- Output: ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails (`CS143/Review`, CONSTRAINT `Review_ibfk_1` FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))
INSERT INTO Review VALUES ("Collin", "2013-03-12 10:31:00", 999999, 4, "Good");

-- Three check constraints
--------------------------

-- Check to make sure the actor's dob is less than actor's dod
-- This modification violates the CHECK because the date of birth for the
-- Actor is greater than its date of death
INSERT INTO Actor VALUES (999999, "Last", "First", "Male", "2012-03-03", "1990-03-03");

-- Check to make sure the actor's sex is either Male or Female
-- This modification violates the CHECK because the actor's sex was not defined 
-- as either "Male" or "Female"
INSERT INTO Actor VALUES (2, "Last", "First", "Error", "2012-03-03", "2013-03-03");


-- Check to make sure the director's dob is less than the director's dod
-- This modification violates the CHECK because the director's date of birth for 
-- the Director is greater than its date of death
INSERT INTO Director VALUES(999999, "Last", "First", "2012-03-03", "2011-03-03");
