package user;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;

public class UserDAO {
	//JSP에서 회원 데이터베이스 테이블에 접근할 수 있도록 생성
	//DAO = Data Access Object, 실질적으로 데이터베이스에서 회원정보를 관리, 처리할때 사용
	
	private Connection conn; 
	//DB 접근 객체
	
	private PreparedStatement pstmt; 
	private ResultSet rs; 
	//정보를 담을 객체
	
	//Ctrl + Shift + O => "Organize Imports" = 클래스에 불필요한 import구문은 제거, 필요한 구문은 자동으로 추가
	
	public UserDAO()
	{
		try
		{
			String dbURL = "jdbc:mysql://localhost:3306/bbs?characterEncoding=UTF-8&serverTimezone=UTC"; 
			//localhost:3306 => 내컴퓨터에 설치된 MySQL, port 3306의 BBS라는 DB에 접속
			
			String dbID = "root";
			String dbPassword = "1234";
			Class.forName("com.mysql.jdbc.Driver"); 
			 //MySQL에 접속할 수 있도록 매개체 역할을 해주는 하나의 라이브러리, JDBC 드라이버 로드
			
			conn = DriverManager.getConnection(dbURL, dbID, dbPassword);
			//DB 접속되면 conn객체에 접속정보가 저장
			
			
		}catch(Exception e)
		{
			e.printStackTrace();
		}
	}
	
	public int login(String userID, String userPassword) //로그인 처리하는 함수
	{
		String SQL = "SELECT userPassword FROM USER WHERE userID = ?";
		try
		{
			pstmt = conn.prepareStatement(SQL);
			pstmt.setString(1, userID);
			rs = pstmt.executeQuery(); //실행 결과를 넣고,
			if(rs.next()) //결과가 존재한다면
			{
				if(rs.getString(1).equals(userPassword)) 
					//결과로 나온 userPassword를 받아서 접속을 시도한 userPassword와 동일하다면
				{
					return 1; //로그인 성공
				}
				else
					return 0; //로그인 실패(비밀번호 틀림)
					
			}
			return -1; //아이디가 없음  userID=?부분 확인
		}catch(Exception e)
		{
			e.printStackTrace();
		}
		return -2; // 데이터베이스 오류
	}
	
	public int join(User user) {
		String SQL = "INSERT INTO USER VALUES(?, ?, ?, ?, ?)";
		try {
			pstmt = conn.prepareStatement(SQL);
			pstmt.setString(1, user.getUserID());
			pstmt.setString(2, user.getUserPassword());
			pstmt.setString(3, user.getUserName());
			pstmt.setString(4, user.getUserGender());
			pstmt.setString(5, user.getUserEmail()); 
			return pstmt.executeUpdate();
		}
		catch(Exception e) {
			e.printStackTrace();
		}
		return -1;
	}
}