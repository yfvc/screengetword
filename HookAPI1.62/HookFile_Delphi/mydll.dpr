//查看VC中的帮助中的File Functions

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

function mySetCurrentDirectoryA(dir:PChar):DWORD;stdcall;
var
  ret:LongBool;
  //err:integer;
begin
  MessageBox(0, dir, 'mySetCurrentDirectory...', MB_OK);
  ret :=SetCurrentDirectoryA(dir);
  //err =GetLastError();
  //加入自己的处理
  //SetLastError(err);
  result :=DWORD(ret);
end;

function mySetCurrentDirectoryW(dir:PWChar):DWORD;stdcall;
var
  ret:LongBool;
  //err:integer;
begin
  MessageBoxW(0, dir, 'mySetCurrentDirectoryW...', MB_OK);
  ret :=SetCurrentDirectoryW(dir);
  //err =GetLastError();
  //加入自己的处理
  //SetLastError(err);
  result :=DWORD(ret);
end;

// 在Win32中，OpenFile被CreateFile代替，所以打开文件时实际上调用了CreateFile
function myOpenFile(fname:PChar; pofs:POFSTRUCT; style:UINT):DWORD;stdcall;
var
  ret:HFILE;
  //err:integer;
begin
  MessageBox(0, fname, 'myOpenFile...', MB_OK);
  ret :=OpenFile(fname, pofs^, style);
  //err =GetLastError();
  //加入自己的处理
  //SetLastError(err);
  Result := ret;
end;

function myCreateFileA(fname:PChar;dwDesiredAccess:DWORD;dwShareMod:DWORD;lpSecurityAttributes:PSecurityAttributes;dwCreationDisposition:DWORD;dwFlagsAndAttributes:DWORD;hTemplateFile:HFILE):DWORD;stdcall;
var
  ret:HFILE;
  //err:integer;
begin
  MessageBox(0, fname, 'myCreateFileA...', MB_OK);
  ret :=CreateFileA(fname, dwDesiredAccess, dwShareMod, lpSecurityAttributes,
		dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
  //err =GetLastError();
  //加入自己的处理
  //SetLastError(err);
  Result := ret;
end;
Function GetMyAPIInfo():Pointer;stdCall;
begin
	GetMyAPIInfo :=Addr(myapi_info);
end;

exports
  GetMyAPIInfo,
  mySetCurrentDirectoryA,
  mySetCurrentDirectoryW,
  myOpenFile,
  myCreateFileA;

begin
  myapi_info[1].module_name :='KERNEL32.DLL';
  myapi_info[1].api_name :='SetCurrentDirectoryA';
  myapi_info[1].param_count :=1;
  myapi_info[1].my_api_name :='mySetCurrentDirectoryA';
  myapi_info[1].start_pos :=0;
  myapi_info[1].friend_my_api_name :=nil;

  myapi_info[2].module_name :='KERNEL32.DLL';
  myapi_info[2].api_name :='OpenFile(a,b,c)';
  myapi_info[2].param_count :=3;
  myapi_info[2].my_api_name :='myOpenFile';
  myapi_info[2].start_pos :=0;
  myapi_info[2].friend_my_api_name :=nil;

  myapi_info[3].module_name :='KERNEL32.DLL';
  myapi_info[3].api_name :='OpenFile(a,b,c)';
  myapi_info[3].param_count :=3;
  myapi_info[3].my_api_name :='myOpenFile';
  myapi_info[3].start_pos :=0;
  myapi_info[3].friend_my_api_name :=nil;

  myapi_info[4].module_name :='KERNEL32.DLL';
  myapi_info[4].api_name :='CreateFileA(1,2,3,4,5,6,7)';
  myapi_info[4].param_count :=7;
  myapi_info[4].my_api_name :='myCreateFileA';
  myapi_info[4].start_pos :=0;
  myapi_info[4].friend_my_api_name :=nil;

  myapi_info[5].module_name :=nil;
  myapi_info[5].api_name :=nil;
  myapi_info[5].my_api_name :=nil;
end.


