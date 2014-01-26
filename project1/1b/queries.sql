-- Define which database should be used
USE CS143;

-- Give me the names of all the actors in the movie 'Die Another Day'.
-- Please also make sure actor names are in this format: 
-- <firstname> <lastname>   (seperated by single space).

SELECT CONCAT(first, ' ', last)
FROM ACTOR,
	(
		SELECT aid
		FROM MovieActor
		WHERE mid =
		(
			SELECT id
			FROM Movie
			WHERE title = "Die Another Day"
		)
	
	) ActorID
WHERE id = ActorID.aid;

-- output appears:
-- CONCAT( first, ' ', last)
-- | John Cleese				|
-- | Paul Darrow				|
-- | Judi Dench					|
-- | Thomas Ho					|
-- | ... and so on				|


-- Give me the count of all the actors who acted in multiple movies.

SELECT COUNT(*)
FROM
	(
		SELECT aid
		FROM MovieActor
		GROUP BY aid
		HAVING COUNT(*) >= 2
	);

