
--------------------------------------------------------------
-- Heldenian Information DB
--------------------------------------------------------------

-- 전면전이 종료 될때 마다 기록한다.
-- index를 걸지 않고 계속 누적된다.
CREATE TABLE HeldenianReportT (
	startDate	DATETIME	NOT NULL,
	endDate		DATETIME	NOT NULL,
	winner		CHAR(7)		NOT NULL
		CHECK (Winner IN ('aresden', 'elvine', 'draw')),
	type		INT		NOT NULL
);

-- 전면전에 대한 기록을 담는다.
-- aresden, elvine, draw 에 대한 각각 하나의 tuple 만을 가진다.
-- index 필요 없음.
CREATE TABLE HeldenianResultT (
	nation			CHAR(7)		NOT NULL UNIQUE
		CHECK (nation IN ('aresden', 'elvine', 'draw')),
	totalWin		INT			DEFAULT 0,
	lastWar			CHAR(4)		NOT NULL
		CHECK (lastWar IN ('lose', 'win', 'draw')),
	lastResultCount	INT			DEFAULT 0	
);

INSERT INTO HeldenianResultT VALUES ('aresden', 0, 'lose', 0);
INSERT INTO HeldenianResultT VALUES ('elvine' , 0, 'lose', 0);
INSERT INTO HeldenianResultT VALUES ('draw'   , 0, 'draw', 0);

-----------------------------
-- stored procedure
-----------------------------

-- save procedure
CREATE PROC sp_SetHeldenianWinner
	@Winner		CHAR(7),
	@StartDate	DATETIME,
	@EndDate	DATETIME,
	@Type		INT
AS
	IF( @Winner = 'aresden' )
	BEGIN
		EXEC sp_SetHeldenianWinnerAresden
		EXEC sp_SetHeldenianLoserElvine
	END
	ELSE IF ( @Winner = 'elvine' )
	BEGIN
		EXEC sp_SetHeldenianWinnerElvine
		EXEC sp_SetHeldenianLoserAresden
	ENd
	ELSE IF ( @Winner = 'draw' )
	BEGIN
		EXEC sp_SetHeldenianDraw
	END

	INSERT INTO HeldenianReportT VALUES ( @StartDate, @EndDate, @Winner, @Type );
GO

CREATE PROC sp_SetHeldenianWinnerAresden
AS
	UPDATE HeldenianResultT SET lastWar='win', lastResultCount=0 WHERE nation='aresden' and lastWar='lose'
	UPDATE HeldenianResultT SET lastResultCount=lastResultCount+1, totalWin=totalWin+1 WHERE nation='aresden'
GO

CREATE PROC sp_SetHeldenianWinnerElvine
AS
	UPDATE HeldenianResultT SET lastWar='win', lastResultCount=0 WHERE nation='elvine' and lastWar='lose'
	UPDATE HeldenianResultT SET lastResultCount=lastResultCount+1, totalWin=totalWin+1 WHERE nation='elvine'
GO


CREATE PROC sp_SetHeldenianLoserAresden
AS
	UPDATE HeldenianResultT SET lastWar='lose', lastResultCount=0 WHERE nation='aresden' and lastWar='win'
	UPDATE HeldenianResultT SET lastResultCount=lastResultCount+1 WHERE nation='aresden'
GO

CREATE PROC sp_SetHeldenianLoserElvine
AS
	UPDATE HeldenianResultT SET lastWar='lose', lastResultCount=0 WHERE nation='elvine' and lastWar='win'
	UPDATE HeldenianResultT SET lastResultCount=lastResultCount+1 WHERE nation='elvine'
GO

CREATE PROC sp_SetHeldenianDraw
AS
	UPDATE HeldenianResultT SET totalWin=totalWin+1 WHERE nation='draw'
GO

-- retrive procedure
CREATE PROC sp_GetHeldenianLastLoser
AS
	SELECT * FROM HeldenianResultT WHERE lastWar='lose'
GO

CREATE PROC sp_GetHeldenianLastWinner
AS
	SELECT * FROM HeldenianResultT WHERE lastWar='win'
GO

