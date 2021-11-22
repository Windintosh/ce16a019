<%@ page contentType="text/html; charset=utf-8"%>
<%@page import="java.io.PrintWriter" %>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width", initial-scale="1">
	<!-- viewport=화면상의 표시영역, content=모바일 장치들에 맞게 크기조정, initial=초기화면 배율 설정 -->
	
<link rel="stylesheet" href="css/bootstrap.css">
	<!-- 스타일시트로 css폴더의 bootstrap.css파일 사용 -->
	
<title>Music</title>
</head>
<body>

	<%@ include file="dbconn.jsp" %>
	<% String main_sub="music"; %>
		<%
		String userID = null;
		if(session.getAttribute("userID") != null)
		{
			userID = (String) session.getAttribute("userID");
		}
			//로그인이 된 회원은 로그인의 정보를 담을수 있도록 설정  
	%>
	<nav class="navbar navbar-inverse"> <!-- navbar-색상(inverse = 검은색, default 22222= 색x) -->
		<div class="navbar-header">
			<button type="button" class="navbar-toggle collapsed"
			data-toggle="collapse" data-target="#bs-example-navbar-collapse-1"
			aria-expanded="false">
				<!-- class="navbar-toggle collapsed"=>네비게이션의 화면 출력유무 
				data-toggle="collapse" : 모바일 상태에서 클릭하면서 메뉴가 나오게 설정 -->
			
				<span class="icon-bar"></span>
				<span class="icon-bar"></span>
				<span class="icon-bar"></span>
				<span class="icon-bar"></span>
					<!-- 아이콘 이미지 -->
				
			</button>
			
			<a class="navbar-brand" href="main.jsp">MuseBee</a>
				<!-- Bootstrap navbar 기본 메뉴바 -->
		</div>
		
		<div class="collapse navbar-collapse" id="bs-example-navbar-collapse-1">
			<ul class="nav navbar-nav"> <!-- navbar-nav => 네비게이션 바의 메뉴 -->
				<li><a href="main.jsp">메인</a></li>
				<li><a href="bbs.jsp">게시판</a></li>
				<li class="active"><a target="_blank" a href="music.jsp">음원재생</a></li>
				<li><a href="sitemap.jsp">사이트맵</a></li>
				<!-- 메뉴, 게시판의 main.jsp와 bbs.jsp의 파일로 각각 이동 -->
			</ul>
			<%
				if(userID == null) //로그인이 되어있지 않았을때, 
				{
					PrintWriter script = response.getWriter();
					 script.println("<script>");
					 script.println("alert('로그인 후 이용 가능합니다.')");
					 script.println("location.href = 'login.jsp'"); 
					 script.println("</script>");
			%>
			<ul class="nav navbar-nav navbar-right">
				<li class="dropdown">
						<a href="#" class="dropdown-toggle"
							data-toggle="dropdown" role="button" aria-haspopup="true"
							aria-expanded="false">접속하기<span class="caret"></span></a>
							<!-- 임시의 주소링크 "#"을 기재한다. -->
							<!-- caret = creates a caret arrow icon (▼) -->
					
						<ul class="dropdown-menu">
							<!-- dropdown-menu : 버튼을 눌렀을때, 생성되는 메뉴(접속하기를 눌렀을때 로그인, 회원가입 메뉴 -->
					
							<li><a href="login.jsp">로그인</a></li>
							<li><a href="join.jsp">회원가입</a></li>
						</ul>
					</li>	
			</ul>
			
			<%
				} else	//로그인이 되었을때
				{
			%>	
			
							<ul class="nav navbar-nav navbar-right">
				<li class="dropdown">
						<a href="#" class="dropdown-toggle"
							data-toggle="dropdown" role="button" aria-haspopup="true"
							aria-expanded="false">회원관리<span class="caret"></span></a>
							<!-- 임시의 주소링크 "#"을 기재한다. -->
							<!-- caret = creates a caret arrow icon (▼) -->
					
						<ul class="dropdown-menu">
							<!-- dropdown-menu : 버튼을 눌렀을때, 생성되는 메뉴(접속하기를 눌렀을때 로그인, 회원가입 메뉴 -->
			
							<li><a href="logoutAction.jsp">로그아웃</a></li>
						</ul>
					</li>	
			</ul>	
			<% 
				}
			%>
			
			</div>
			</nav>
	<h1 style="margin-left:10%">Music Board</h1>
	
	<form style="float:right; margin-right:20% "id='gopage' method='post' action='add.jsp'> 
	<input type="submit" value="음원추가">
	</form>
	 
	<table style="margin-left:10%;
	text-align:center";>
	<thead>
	<tr>
	<th width=100px>No.   </th>
	<th width=100px>음원명    </th>
	<th width=100px>가수    </th>
	<th width=550px>노래    </th>
	<th width=30px>    </th>
	</tr>
	</thead>
	<tbody style="margin-left:10%;text-align:left";>
	<%	PreparedStatement pstmt = null;
	ResultSet rs=null;
	try {
		String sql = "select * from music order by id desc";
		pstmt = conn.prepareStatement(sql);

		rs=pstmt.executeQuery();
		
		while(rs.next()){
		String id=rs.getString("id");
		String musicname=rs.getString("musicname");
		String singer=rs.getString("singer");
		String mp3=rs.getString("mp3");
		%>
		<tr>
			<td><%=id %></td>
			<td><%=musicname %></a></td>
			<td><%=singer %></td>
			<td><audio controls> 
			<source src="http://localhost:8080/BMS_JSP/mp3file/<%=mp3%>" type="audio/mp3"></audio></td>
			<td><form  method='post' action='delete_process.jsp' >
			<input type="hidden" name="number" value="<%=id %>" />
			<input type="submit"name="delete_music" value="X"/></form>
			</td>
		</tr>
	
		<%
		}
	} catch (SQLException ex) {
		out.println("SQLException: " + ex.getMessage());
	} finally {
		if (pstmt != null)
			pstmt.close();
		if (conn != null)
		conn.close();
	}
	 %>
	
	</tbody>
	</table>
		<script src="https://code.jquery.com/jquery-3.1.1.min.js"></script>
	<script src="js/bootstrap.js"></script>
	
</body>
</html>