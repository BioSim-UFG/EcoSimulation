program TestZipStream;

{$APPTYPE CONSOLE}

{$R *.res}

uses
  System.SysUtils,
  System.Classes,
  ZLib,
  LibTypes,
  LibMatrix;

Var
  OutStream: TMemoryStream;
  ZipStream: TMemoryStream;
  i, j, n, m: Integer;
  Vec: TIntVector;

begin
  try

   SetLength(Vec, 20);
   Vec[00]:= 2;
   Vec[01]:= 3;
   Vec[02]:= 4;
   Vec[03]:= 7;
   Vec[04]:= 11;
   Vec[05]:= 13;
   Vec[06]:= 17;
   Vec[07]:= 19;
   Vec[08]:= 23;
   Vec[09]:= 31;
   Vec[10]:= 37;
   Vec[11]:= 41;
   Vec[12]:= 43;
   Vec[13]:= 47;
   Vec[14]:= 53;
   Vec[15]:= 59;
   Vec[16]:= 61;
   Vec[17]:= 67;
   Vec[18]:= 71;
   Vec[19]:= 73;

   For i:= 1000 downto 1 do
    begin
     SetLength(Vec, Length(Vec)+1);
     Vec[Length(Vec)-1]:= i;
    end;

   OutStream:= TMemoryStream.Create;
   OutStream.Position:= 0;

   For i:= 0 to Length(Vec)-1 do
    begin
     m:= Vec[i];
     OutStream.Write(m, SizeOf(Integer));
    end;

    ZipStream:= TMemoryStream.Create;

    OutStream.Position:= 0;
    ZCompressStream(OutStream, ZipStream);

    ZipStream.Position:= 0;
    ZipStream.SaveToFile('TestePrimes.StreamZip');
    FreeAndNil(ZipStream);

  except
    on E: Exception do
      Writeln(E.ClassName, ': ', E.Message);
  end;
end.
