package bbs;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.util.ArrayList;

public class BbsDAO {
	//JSP에서 회원 데이터베이스 테이블에 접근할 수 있도록 생성
	//DAO = Data Access Object, 실질적으로 데이터베이스에서 회원정보를 관리, 처리할때 사용
	
	private Connection conn; 
	//DB 접근 객체
	
	private PreparedStatement pstmt; 
	private ResultSet rs; 
	//정보를 담을 객체
	
	//Ctrl + Shift + O => "Organize Imports" = 클래스에 불필요한 import구문은 제거, 필요한 구문은 자동으로 추가
	
	public BbsDAO()
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

	public String getDate() //게시판에 글을 작성할 때 현재의 시간을 가져오는 함수
	{
		String SQL = "SELECT NOW()"; //현재의 시간을 가져오는 MySQL 문장
		try {
			PreparedStatement pstmt = conn.prepareStatement(SQL);
			rs = pstmt.executeQuery();
			
			if(rs.next()) {
				return rs.getString(1); //현재의 시간 반환
			}
				
		} catch(Exception e) {
			e.printStackTrace();
		}
		return ""; //데이텁데이스 오류
	}
	
	public int getNext() // 게시글 번호 입력 함수
	{
		String SQL = "SELECT bbsID FROM BBS ORDER BY bbsID DESC"; 
		//게시글 번호는 1번부터 2번, 3, 4, ...올라가서 마지막의 번호를 가져와서 +1해준다.
		try {
			PreparedStatement pstmt = conn.prepareStatement(SQL);
			rs = pstmt.executeQuery();
			
			if(rs.next()) {
				return rs.getInt(1) + 1; //마지막의 게시글 번호에서 +1해준다.
			} 
			return 1; //첫번째 게시물인 경우
				
		} catch(Exception e) {
			e.printStackTrace();
		}
		return -1; //데이터베이스 오류
	}
	
	public int write(String bbsTitle, String userID, String bbsContent) //실제 글을 작성하는 함수
	{
		String SQL = "INSERT INTO BBS VALUES (?, ?, ?, ?, ?, ?)"; 
		//SQL문 값넣는 문장
		try {
			PreparedStatement pstmt = conn.prepareStatement(SQL);
			pstmt.setInt(1, getNext()); //bbsID
			pstmt.setString(2, bbsTitle);
			pstmt.setString(3, userID);
			pstmt.setString(4, getDate());
			pstmt.setString(5, bbsContent);
			pstmt.setInt(6, 1); //처음 보여지는상태 => 1
			
			//rs = pstmt.executeQuery();
			return pstmt.executeUpdate();
				
		} catch(Exception e) {
			e.printStackTrace();
		}
		return -1; //데이터베이스 오류
	}
	
	public ArrayList<Bbs> getList(int pageNumber){ 
		String SQL = "SELECT * FROM BBS WHERE bbsID < ? AND bbsAvailable = 1 ORDER BY bbsID DESC LIMIT 10;";
		ArrayList<Bbs> list = new ArrayList<Bbs>();
		try {
			PreparedStatement pstmt = conn.prepareStatement(SQL);
			pstmt.setInt(1, getNext() - (pageNumber -1) * 10);
			rs = pstmt.executeQuery();
			while (rs.next()) {
				Bbs bbs = new Bbs();
				bbs.setBbsID(rs.getInt(1));
				bbs.setBbsTitle(rs.getString(2));
				bbs.setUserID(rs.getString(3));
				bbs.setBbsDate(rs.getString(4));
				bbs.setBbsContent(rs.getString(5));
				bbs.setBbsAvailable(rs.getInt(6));
				list.add(bbs);
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return list; 
	}	
	
	//10 단위 페이징 처리를 위한 함수
	public boolean nextPage(int pageNumber) //페이징 처리
	{
		String SQL = "SELECT * FROM BBS WHERE bbsID < ? AND bbsAvailable = 1 ORDER BY bbsID DESC LIMIT 10;"; 
		//bbsID가 특정조건보다 작을때, Available=1 (삭제가 되지 않고), 10개까지 제한해서 나타냄
		
		try {
			PreparedStatement pstmt = conn.prepareStatement(SQL);
			pstmt.setInt(1, getNext() - (pageNumber - 1) * 10);
			/*getNext = 다음에 작성될 글의번호, 만약 2page, 15번이라면  15 - (2-1)*10 =5, 즉 2page 1, 2, 3, 4 총 4개의 글이 나타남 
			만약 1page, 6번이라면 6 - (1-1)*10 = 6, 즉 1page 1, 2, 3, 4 ,5 총 5개의 글이 나타남 */	
			rs = pstmt.executeQuery();			
			if(rs.next()) { 	//값이 존재할동안,
				return true;
			} 		
		} catch(Exception e) {
			e.printStackTrace();
		}
		return false;
	}
	public Bbs getBbs(int bbsID) {
		String SQL="SELECT * FROM BBS WHERE bbsID = ?";
		try {
			PreparedStatement pstmt = conn.prepareStatement(SQL);
			pstmt.setInt(1, bbsID);
			rs= pstmt.executeQuery();
			
			if(rs.next()) {
				Bbs bbs = new Bbs();
				bbs.setBbsID(rs.getInt(1));
				bbs.setBbsTitle(rs.getString(2));
				bbs.setUserID(rs.getString(3));
				bbs.setBbsDate(rs.getString(4));
				bbs.setBbsContent(rs.getString(5));
				bbs.setBbsAvailable(rs.getInt(6));
				return bbs;
			}
		} catch(Exception e) {
			e.printStackTrace();
		}
		return null;
	}
	
	public int update(int bbsID, String bbsTitle, String bbsContent) {
		String SQL = "UPDATE BBS SET bbsTitle = ?, bbsContent = ? WHERE bbsID = ?";
		try {
			PreparedStatement pstmt = conn.prepareStatement(SQL);
			pstmt.setString(1, bbsTitle);
			pstmt.setString(2, bbsContent);
			pstmt.setInt(3, bbsID);
			return pstmt.executeUpdate();

		} catch (Exception e) {
			e.printStackTrace();
		}
		return -1; // 데이터베이스 오류
	}
	//삭제 함수
		public int delete(int bbsID) {
			String SQL = "UPDATE BBS SET bbsAvailable = 0 WHERE bbsID = ?";
			try {
				PreparedStatement pstmt = conn.prepareStatement(SQL);   
				pstmt.setInt(1, bbsID);
				return pstmt.executeUpdate();
			} catch (Exception e) {
				e.printStackTrace();
			}
			return -1; // 데이터베이스 오류
		}
		
}