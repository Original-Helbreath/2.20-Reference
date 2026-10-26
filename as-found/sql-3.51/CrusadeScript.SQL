
--------------------------------------------------------------
-- Crusade Information DB
--------------------------------------------------------------

-- 전면전이 종료 될때 마다 기록한다.
-- index를 걸지 않고 계속 누적된다.
CREATE TABLE CrusadeReportT (
	startDate	DATETIME	NOT NULL,
	endDate		DATETIME	NOT NULL,
	winner		CHAR(7)		NOT NULL
		CHECK (Winner IN ('aresden', 'elvine', 'draw'))
);

-- 전면전에 대한 기록을 담는다.
-- aresden, elvine, draw 에 대한 각각 하나의 tuple 만을 가진다.
-- index 필요 없음.
CREATE TABLE CrusadeResultT (
	nation			CHAR(7)		NOT NULL UNIQUE
		CHECK (nation IN ('aresden', 'elvine', 'draw')),
	totalWin		INT			DEFAULT 0,
	lastWar			CHAR(4)		NOT NULL
		CHECK (lastWar IN ('lose', 'win', 'draw')),
	lastResultCount	INT			DEFAULT 0	
);

INSERT INTO CrusadeResultT VALUES ('aresden', 0, 'lose', 0);
INSERT INTO CrusadeResultT VALUES ('elvine' , 0, 'lose', 0);
INSERT INTO CrusadeResultT VALUES ('draw'   , 0, 'draw', 0);

-----------------------------
-- stored procedure
-----------------------------

-- save procedure
CREATE PROC sp_SetCrusadeWinner
	@Winner		CHAR(7),
	@StartDate	DATETIME,
	@EndDate	DATETIME
AS
	IF( @Winner = 'aresden' )
	BEGIN
		EXEC sp_SetCrusadeWinnerAresden
		EXEC sp_SetCrusadeLoserElvine
	END
	ELSE IF ( @Winner = 'elvine' )
	BEGIN
		EXEC sp_SetCrusadeWinnerElvine
		EXEC sp_SetCrusadeLoserAresden
	ENd
	ELSE IF ( @Winner = 'draw' )
	BEGIN
		EXEC sp_SetCrusadeDraw
	END

	INSERT INTO CrusadeReportT VALUES ( @StartDate, @EndDate, @Winner );
GO

CREATE PROC sp_SetCrusadeWinnerAresden
AS
	UPDATE CrusadeResultT SET lastWar='win', lastResultCount=0 WHERE nation='aresden' and lastWar='lose'
	UPDATE CrusadeResultT SET lastResultCount=lastResultCount+1, totalWin=totalWin+1 WHERE nation='aresden'
GO

CREATE PROC sp_SetCrusadeWinnerElvine
AS
	UPDATE CrusadeResultT SET lastWar='win', lastResultCount=0 WHERE nation='elvine' and lastWar='lose'
	UPDATE CrusadeResultT SET lastResultCount=lastResultCount+1, totalWin=totalWin+1 WHERE nation='elvine'
GO


CREATE PROC sp_SetCrusadeLoserAresden
AS
	UPDATE CrusadeResultT SET lastWar='lose', lastResultCount=0 WHERE nation='aresden' and lastWar='win'
	UPDATE CrusadeResultT SET lastResultCount=lastResultCount+1 WHERE nation='aresden'
GO

CREATE PROC sp_SetCrusadeLoserElvine
AS
	UPDATE CrusadeResultT SET lastWar='lose', lastResultCount=0 WHERE nation='elvine' and lastWar='win'
	UPDATE CrusadeResultT SET lastResultCount=lastResultCount+1 WHERE nation='elvine'
GO

CREATE PROC sp_SetCrusadeDraw
AS
	UPDATE CrusadeResultT SET totalWin=totalWin+1 WHERE nation='draw'
GO

-- retrive procedure
CREATE PROC sp_GetCrusadeLastLoser
AS
	SELECT * FROM CrusadeResultT WHERE lastWar='lose'
GO

CREATE PROC sp_GetCrusadeLastWinner
AS
	SELECT * FROM CrusadeResultT WHERE lastWar='win'
GO

