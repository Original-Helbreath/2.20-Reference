use ws1 
go

dbcc dbreindex(character_t, ' ',80)
go
dbcc dbreindex(skill_t, ' ',80)
go
dbcc dbreindex(item_t, ' ',70)
go
dbcc dbreindex(bankitem_t, ' ',70)
go
dbcc dbreindex(guild_t, ' ',70)
go
dbcc dbreindex(guildmember_t, ' ',70)
go
