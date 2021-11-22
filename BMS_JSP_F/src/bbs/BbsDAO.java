package bbs;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.util.ArrayList;

public class BbsDAO {
	//JSP���� ȸ�� �����ͺ��̽� ���̺� ������ �� �ֵ��� ����
	//DAO = Data Access Object, ���������� �����ͺ��̽����� ȸ�������� ����, ó���Ҷ� ���
	
	private Connection conn; 
	//DB ���� ��ü
	
	private PreparedStatement pstmt; 
	private ResultSet rs; 
	//������ ���� ��ü
	
	//Ctrl + Shift + O => "Organize Imports" = Ŭ������ ���ʿ��� import������ ����, �ʿ��� ������ �ڵ����� �߰�
	
	public BbsDAO()
	{
		try
		{
			String dbURL = "jdbc:mysql://localhost:3306/bbs?characterEncoding=UTF-8&serverTimezone=UTC"; 
			//localhost:3306 => ����ǻ�Ϳ� ��ġ�� MySQL, port 3306�� BBS��� DB�� ����
			
			String dbID = "root";
			String dbPassword = "1234";
			Class.forName("com.mysql.jdbc.Driver"); 
			 //MySQL�� ������ �� �ֵ��� �Ű�ü ������ ���ִ� �ϳ��� ���̺귯��, JDBC ����̹� �ε�
			
			conn = DriverManager.getConnection(dbURL, dbID, dbPassword);
			//DB ���ӵǸ� conn��ü�� ���������� ����
			
			
		}catch(Exception e)
		{
			e.printStackTrace();
		}
	}
	
	public int login(String userID, String userPassword) //�α��� ó���ϴ� �Լ�
	{
		String SQL = "SELECT userPassword FROM USER WHERE userID = ?";
		try
		{
			pstmt = conn.prepareStatement(SQL);
			pstmt.setString(1, userID);
			rs = pstmt.executeQuery(); //���� ����� �ְ�,
			if(rs.next()) //����� �����Ѵٸ�
			{
				if(rs.getString(1).equals(userPassword)) 
					//����� ���� userPassword�� �޾Ƽ� ������ �õ��� userPassword�� �����ϴٸ�
				{
					return 1; //�α��� ����
				}
				else
					return 0; //�α��� ����(��й�ȣ Ʋ��)
					
			}
			return -1; //���̵� ����  userID=?�κ� Ȯ��
		}catch(Exception e)
		{
			e.printStackTrace();
		}
		return -2; // �����ͺ��̽� ����
	}

	public String getDate() //�Խ��ǿ� ���� �ۼ��� �� ������ �ð��� �������� �Լ�
	{
		String SQL = "SELECT NOW()"; //������ �ð��� �������� MySQL ����
		try {
			PreparedStatement pstmt = conn.prepareStatement(SQL);
			rs = pstmt.executeQuery();
			
			if(rs.next()) {
				return rs.getString(1); //������ �ð� ��ȯ
			}
				
		} catch(Exception e) {
			e.printStackTrace();
		}
		return ""; //�����ӵ��̽� ����
	}
	
	public int getNext() // �Խñ� ��ȣ �Է� �Լ�
	{
		String SQL = "SELECT bbsID FROM BBS ORDER BY bbsID DESC"; 
		//�Խñ� ��ȣ�� 1������ 2��, 3, 4, ...�ö󰡼� �������� ��ȣ�� �����ͼ� +1���ش�.
		try {
			PreparedStatement pstmt = conn.prepareStatement(SQL);
			rs = pstmt.executeQuery();
			
			if(rs.next()) {
				return rs.getInt(1) + 1; //�������� �Խñ� ��ȣ���� +1���ش�.
			} 
			return 1; //ù��° �Խù��� ���
				
		} catch(Exception e) {
			e.printStackTrace();
		}
		return -1; //�����ͺ��̽� ����
	}
	
	public int write(String bbsTitle, String userID, String bbsContent) //���� ���� �ۼ��ϴ� �Լ�
	{
		String SQL = "INSERT INTO BBS VALUES (?, ?, ?, ?, ?, ?)"; 
		//SQL�� ���ִ� ����
		try {
			PreparedStatement pstmt = conn.prepareStatement(SQL);
			pstmt.setInt(1, getNext()); //bbsID
			pstmt.setString(2, bbsTitle);
			pstmt.setString(3, userID);
			pstmt.setString(4, getDate());
			pstmt.setString(5, bbsContent);
			pstmt.setInt(6, 1); //ó�� �������»��� => 1
			
			//rs = pstmt.executeQuery();
			return pstmt.executeUpdate();
				
		} catch(Exception e) {
			e.printStackTrace();
		}
		return -1; //�����ͺ��̽� ����
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
	
	//10 ���� ����¡ ó���� ���� �Լ�
	public boolean nextPage(int pageNumber) //����¡ ó��
	{
		String SQL = "SELECT * FROM BBS WHERE bbsID < ? AND bbsAvailable = 1 ORDER BY bbsID DESC LIMIT 10;"; 
		//bbsID�� Ư�����Ǻ��� ������, Available=1 (������ ���� �ʰ�), 10������ �����ؼ� ��Ÿ��
		
		try {
			PreparedStatement pstmt = conn.prepareStatement(SQL);
			pstmt.setInt(1, getNext() - (pageNumber - 1) * 10);
			/*getNext = ������ �ۼ��� ���ǹ�ȣ, ���� 2page, 15���̶��  15 - (2-1)*10 =5, �� 2page 1, 2, 3, 4 �� 4���� ���� ��Ÿ�� 
			���� 1page, 6���̶�� 6 - (1-1)*10 = 6, �� 1page 1, 2, 3, 4 ,5 �� 5���� ���� ��Ÿ�� */	
			rs = pstmt.executeQuery();			
			if(rs.next()) { 	//���� �����ҵ���,
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
		return -1; // �����ͺ��̽� ����
	}
	//���� �Լ�
		public int delete(int bbsID) {
			String SQL = "UPDATE BBS SET bbsAvailable = 0 WHERE bbsID = ?";
			try {
				PreparedStatement pstmt = conn.prepareStatement(SQL);   
				pstmt.setInt(1, bbsID);
				return pstmt.executeUpdate();
			} catch (Exception e) {
				e.printStackTrace();
			}
			return -1; // �����ͺ��̽� ����
		}
		
}