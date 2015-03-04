// 请使用ftp.exe测试
library mydll;

uses
  SysUtils, Types, Windows,WinSock,
  Classes;

type
  TMyAPIInfo = record
  module_name:PChar;
  api_name:PChar;
  param_count:integer;
  my_api_name:PChar;
  start_pos:integer;
  friend_my_api_name:PChar;
  end;
{$R *.res}
var
  myapi_info:array[1..5] of TMyAPIInfo;

function mysocket(af:integer; stype:integer; protocol:integer):integer;stdcall;
var
  ret:integer;
  err:integer;
  p:PChar;
  temp:String;
  temp1:array[1..256] of Char;
  i:integer;
begin
  {if count =3 then
    MessageBox(0, 'params count=3', 'mydll-mysocket', MB_OK)
  else
    MessageBox(0, 'params count<>3', 'mydll-mysocket', MB_OK);
  MessageBox(0, 'mysocket run...', 'mydll-mysocket', MB_OK);
  }

  temp :='af='+IntToStr(af)+',type='+IntToStr(stype)+',protocol='+IntToStr(protocol)+#0;
  for i:=1 to Length(temp) do
    temp1[i] :=temp[i];
  p :=@temp1;
  MessageBox(0, p, 'mydll-mysocket', MB_OK);

  ret :=socket(af, stype, protocol);

  err :=WSAGetLastError(); //GetLastError()
  temp :='socket ok. ret='+IntToStr(ret)+',err='+IntToStr(err)+#0;
  for i:=1 to Length(temp) do
    temp1[i] :=temp[i];
  p :=@temp1;
  MessageBox(0, p, 'mydll-mysocket', MB_OK);

  WSASetLastError(err);
  Result := ret;
end;

function myconnect(s:integer; name:PSockAddrIn; namelen:integer):DWORD;stdcall;
var
  ret:integer;
  err:integer;
  temp:String;
  temp1:array[1..256] of Char;
  p:PChar;
  i:integer;
begin
  {if count =3 then
    MessageBox(0, 'params count=3', 'mydll-myconnect', MB_OK);
  MessageBox(0, 'myconnect run...', 'mydll-myconnect', MB_OK);
  }
  temp :='s='+IntToStr(s)+',port='+IntToStr(ntohs(name.sin_port))+',addr='+Inet_Ntoa(name.sin_addr)+',namelen='+IntToStr(namelen)+#0;
  for i:=1 to Length(temp) do
    temp1[i] :=temp[i];
  p :=@temp1;
  MessageBox(0, p, 'mydll-myconnect', MB_OK);

  ret :=connect(s, name^, namelen);
  err :=WSAGetLastError();
  temp :='ret='+IntToStr(ret)+',err='+IntToStr(err)+#0;
  for i:=1 to Length(temp) do
    temp1[i] :=temp[i];
  p :=@temp1;
  MessageBox(0, p, 'mydll-myconnect', MB_OK);
  WSASetLastError(err);
  Result := DWORD(ret);
end;

function myrecv(s:integer; buf:PChar; len:integer; flags:integer):DWORD;stdcall;
var
  p:PChar;
  temp:String;
  temp1:array[1..256] of Char;
  buf_temp:array[1..5000] of Char;
  i,ret,err:integer;
begin
  {if count =4 then
    MessageBox(0, 'params count=4', 'mydll-myrecv', MB_OK);
  MessageBox(0, 'myrecv run...', 'mydll-myrecv', MB_OK);
  }
  err :=0;
  if len >4096 then
    len :=4096;

  ret :=recv(s, buf_temp, len, flags);
  if ret <=0 then
  begin
    err :=WSAGetLastError();
    temp :='ret='+IntToStr(DWORD(ret))+',err='+IntToStr(err)+#0;
    for i:=1 to Length(temp) do
      temp1[i] :=temp[i];
    p :=@temp1;
    MessageBox(0, p, 'mydll-myrecv', MB_OK);
  end;
  if ret >0 then
  begin
    for i :=1 to ret do
    begin
      (buf+i-1)^ :=buf_temp[i];
    end;
  end;
  MessageBox(0, 'myrecv ok', 'myrecv', MB_OK);

  // 对于二进制数据，请不要使用此语句显示数据
  //MessageBox(0, buf, 'myrecv', MB_OK);
  if ret <=0 then
    WSASetLastError(err);
  result :=DWORD(ret);
end;

function mysend(s:integer; buf:PChar; len:integer; flags:integer):DWORD;stdcall;
var
  p:PChar;
  temp:String;
  temp1:array[1..256] of Char;
  buf_temp:array[1..4096] of Char;
  i,ret,err:integer;
begin
  {if count =4 then
    MessageBox(0, 'params count=4', 'mydll-mysend', MB_OK);
  MessageBox(0, 'mysend run...', 'mydll-mysend', MB_OK);
  }
  // 对于二进制数据，请不要使用此语句显示
  //MessageBox(0, buf, 'mysend', MB_OK);

  for i :=1 to len do
  begin
    buf_temp[i] :=(buf+i-1)^;
  end;
  //if len >4096...
  ret :=send(s, buf_temp, len, flags);
  err :=WSAGetLastError();
  //MessageBox(0, 'send ok', 'mysend', MB_OK);
  temp :='ret='+IntToStr(ret)+',err='+IntToStr(err)+#0;
  for i:=1 to Length(temp) do
    temp1[i] :=temp[i];
  p :=@temp1;
  MessageBox(0, p, 'mydll-mysend', MB_OK);
  WSASetLastError(err);
  result :=DWORD(ret);
end;

Function GetMyAPIInfo():Pointer;stdCall;
begin
	GetMyAPIInfo :=Addr(myapi_info);
end;

exports
  GetMyAPIInfo,
  mysocket,
  myconnect,
  mysend,
  myrecv;

begin

  myapi_info[1].module_name :='WSOCK32.DLL';
  myapi_info[1].api_name :='socket';
  myapi_info[1].param_count :=3;
  myapi_info[1].my_api_name :='mysocket';
  myapi_info[1].start_pos :=0;
  myapi_info[1].friend_my_api_name :=nil;

  myapi_info[2].module_name :='WSOCK32.DLL';
  myapi_info[2].api_name :='connect';
  myapi_info[2].param_count :=3;
  myapi_info[2].my_api_name :='myconnect';
  myapi_info[2].start_pos :=0;
  myapi_info[2].friend_my_api_name :=nil;

  myapi_info[3].module_name :='WSOCK32.DLL';
  myapi_info[3].api_name :='send';
  myapi_info[3].param_count :=4;
  myapi_info[3].my_api_name :='mysend';
  myapi_info[3].start_pos :=0;
  myapi_info[3].friend_my_api_name :=nil;

  myapi_info[4].module_name :='WSOCK32.DLL';
  myapi_info[4].api_name :='recv(SOCKET, buf, int, int)';
  myapi_info[4].param_count :=4;
  myapi_info[4].my_api_name :='myrecv';
  myapi_info[4].start_pos :=0;
  myapi_info[4].friend_my_api_name :=nil;

  myapi_info[5].module_name :=nil;
  myapi_info[5].api_name :=nil;
  myapi_info[5].my_api_name :=nil;
end.

