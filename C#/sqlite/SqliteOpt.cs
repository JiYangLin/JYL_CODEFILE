using System.Data.SQLite;
using System.IO;


namespace SqliteOptSpace
{
    class SqliteOpt
    {
        SQLiteConnection m_dbConnection = null;

        public void CreateDatabase(string databasePathName)
        {
            FileInfo inf = new FileInfo(databasePathName);
            if (inf.Exists) inf.Delete();
            SQLiteConnection.CreateFile(databasePathName);

            OpenDatabase(databasePathName);
        }
        public void OpenDatabase(string databasePathName)
        {
            m_dbConnection = new SQLiteConnection(string.Format("Data Source={0};Version=3;", databasePathName));
            m_dbConnection.Open();
        }
        public void ExecSql(string sql)
        {
            if (null == m_dbConnection) return;

            SQLiteCommand command = new SQLiteCommand(sql, m_dbConnection);
            command.ExecuteNonQuery();
        }
        public SQLiteDataReader Select(string sql)
        {
            SQLiteCommand command = new SQLiteCommand(sql, m_dbConnection);
            return command.ExecuteReader();
        }
    }
}


//SqliteOpt opt = new SqliteOpt();
//opt.OpenDatabase(@"D:\...\DataBase.db");
//SQLiteDataReader reader = opt.Select("SELECT * FROM BHBaseInfor_Table");
//while(reader.Read())
//{
//      string val = (string)reader["RoadName"];
//      Int64 id = (Int64)reader["ID"];
//}