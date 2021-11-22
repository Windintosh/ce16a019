<%@ page contentType="text/html; charset=utf-8"%>
<%@ page import="java.sql.*"%>
<%@ page import="java.util.*"%>
<%@ page import="java.io.*"%>
<%@ page import="java.util.Calendar" %>
<%@ page import="java.text.SimpleDateFormat" %>
<%@ page import="com.oreilly.servlet.*"%>
<%@ page import="com.oreilly.servlet.multipart.*"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c" %>
<%@ taglib uri="http://java.sun.com/jsp/jstl/sql" prefix="sql" %>
<html>
<head>
<title>add process</title>
</head>
<body>
	<%@ include file="dbconn.jsp" %>
	<%
		request.setCharacterEncoding("utf-8");
		String mp3 = "";
		String realFolder = "C:\\Users\\MWS\\eclipse-workspace\\BMS_JSP_F\\WebContent\\mp3file"; //웹 어플리케이션상의 절대 경로
		String encType = "utf-8"; //인코딩 타입
		int maxSize = 50 * 1024 * 1024; //최대 업로드될 파일의 크기50Mb
		MultipartRequest multi = new MultipartRequest(request, realFolder, maxSize, encType, new DefaultFileRenamePolicy());
		
		String musicname = multi.getParameter("musicname");
		String singer = multi.getParameter("singer");
		
		Enumeration files = multi.getFileNames();
		String fname = (String) files.nextElement();
		mp3 = multi.getFilesystemName(fname);
		
	%>
	<sql:setDataSource var="dataSource"
		url ="jdbc:mysql://localhost:3306/bbs?useUnicode=true&useJDBCCompliantTimezoneShift=true&useLegacyDatetimeCode=false&serverTimezone=UTC&characterEncoding=utf-8"
		driver = "com.mysql.jdbc.Driver" user="root" password="1234" />
		<sql:update dataSource="${dataSource}" var="resultSet">
		INSERT INTO music(musicname, mp3, singer) VALUES (?,?,?)
		<sql:param value="<%=musicname %>" />
		<sql:param value="<%=mp3 %>" />
		<sql:param value="<%=singer %>" />
		
		</sql:update>
		
		<c:redirect url="music.jsp" />
		
</body>
</html>
