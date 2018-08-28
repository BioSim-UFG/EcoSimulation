unit LibTetraIO;

interface

Uses
  SysUtils,
  Classes,
  Math,
  ZLib,

  LibTetraSim,
  LibTetraData,
  LibTetraCells,

  LibTypes;

  Function WriteSimData(SimData: TSimData): Boolean;
  Function ReadSimData(SimData: TSimData): Boolean;

implementation




Procedure WriteString(OutStream: TMemoryStream; Str: String);
var
 k: Integer;
begin
 k:= Length(Str);                          // String
 OutStream.Write(k, SizeOf(Integer));
 If k > 0 then
   OutStream.Write(Pointer(Str)^, k*SizeOf(Char));
end;

Function ReadString(InStream: TMemoryStream): String;
var
 k: Integer;
 TmpStr: String;
begin
 Result:= '';
 InStream.Read(k, SizeOf(Integer)); // String
 SetLength(TmpStr, k);
 If k > 0 then
  begin
   InStream.Read(Pointer(TmpStr)^, k*SizeOf(Char));

   Result:= TmpStr;
  end;
end;




Function WriteSimData(SimData: TSimData): Boolean;
 var
  OutStream: TMemoryStream;
  ZipStream: TMemoryStream;
  MapCell: PMapCell;
  i, j, t, k: Integer;
 begin
  Result:= False;

  Try
    OutStream:= TMemoryStream.Create;
    OutStream.Position:= 0;

    // Write paths and files
    WriteString(OutStream, SimData.DataParams.DataPath);
    WriteString(OutStream, SimData.DataParams.SimPath);
    WriteString(OutStream, SimData.DataParams.ResPath);
    WriteString(OutStream, SimData.DataParams.GeoDistFile);
    WriteString(OutStream, SimData.DataParams.TopoDistFile);
    WriteString(OutStream, SimData.DataParams.RiversDistFile);
    WriteString(OutStream, SimData.DataParams.CellAreaVecFile);

    OutStream.Write(SimData.TotNumCells, SizeOf(Integer));
    OutStream.Write(SimData.TotNumEnvVars, SizeOf(Integer));
    OutStream.Write(SimData.TotNumTimeSteps, SizeOf(Integer));

    For i:= 0 to SimData.TotNumCells-1 do
     begin
      OutStream.Write(SimData.Grid.Cells[i].IdCell, SizeOf(Integer));
      OutStream.Write(SimData.Grid.Cells[i].Area, SizeOf(Single));

      For j:= 0 to SimData.TotNumCells-1 do
       begin
        OutStream.Write(SimData.Grid.Cells[i].GeoConn[j], SizeOf(Single));
        OutStream.Write(SimData.Grid.Cells[i].TopoConn[j], SizeOf(Single));
        OutStream.Write(SimData.Grid.Cells[i].RiversConn[j], SizeOf(Single));
       end;
     end;

    ForceDirectories(SimData.DataParams.DataPath);

    ZipStream:= TMemoryStream.Create;

    OutStream.Position:= 0;
    ZCompressStream(OutStream, ZipStream);

    ZipStream.Position:= 0;

    ZipStream.SaveToFile(SimData.DataParams.DataPath + '!SimData.sim');

    FreeAndNil(ZipStream);
    FreeAndNil(OutStream);

  Except
    Exception.Create(#13#10 + #13#10 + #13#10 +
                     'Could not write simulation data!' + #13#10 +
                     'Press any key to continue...');
    ReadLn(i);
   End;

  Result:= True;
 end;


Function ReadSimData(SimData: TSimData): Boolean;
 var
  InStream: TMemoryStream;
  ZipStream: TMemoryStream;
  i, j, t, k, l: Integer;
 begin
  Result:= False;

  Try
    If not FileExists(SimData.DataParams.DataPath + '!SimData.sim') then
     begin
      Exception.Create(#13#10 + #13#10 + #13#10 +
                       'Could not find the simulation data file!' + #13#10 +
                       'Press any key to continue...');
      ReadLn(i);
     end;

    ZipStream:= TMemoryStream.Create;
    ZipStream.Position:= 0;
    ZipStream.LoadFromFile(SimData.DataParams.DataPath + '!SimData.sim');

    InStream:= TMemoryStream.Create;
    ZDecompressStream(ZipStream, InStream);
    InStream.Position:= 0;

    ZipStream.Free;

    SimData.DataParams.DataPath:= ReadString(InStream);
    SimData.DataParams.SimPath:= ReadString(InStream);
    SimData.DataParams.ResPath:= ReadString(InStream);
    SimData.DataParams.GeoDistFile:= ReadString(InStream);
    SimData.DataParams.TopoDistFile:= ReadString(InStream);
    SimData.DataParams.RiversDistFile:= ReadString(InStream);
    SimData.DataParams.CellAreaVecFile:= ReadString(InStream);

    InStream.Read(SimData.TotNumCells, SizeOf(Integer));
    InStream.Read(SimData.TotNumEnvVars, SizeOf(Integer));
    InStream.Read(SimData.TotNumTimeSteps, SizeOf(Integer));

    SetLength(SimData.Grid.Cells, SimData.TotNumCells);
    For i:= 0 to SimData.TotNumCells-1 do
     begin
      InStream.Read(SimData.Grid.Cells[i].IdCell, SizeOf(Integer));
      inStream.Read(SimData.Grid.Cells[i].Area, SizeOf(Single));

      SetLength(SimData.Grid.Cells[i].GeoConn, SimData.TotNumCells);
      SetLength(SimData.Grid.Cells[i].TopoConn, SimData.TotNumCells);
      SetLength(SimData.Grid.Cells[i].RiversConn, SimData.TotNumCells);
      For j:= 0 to SimData.TotNumCells-1 do
       begin
        InStream.Read(SimData.Grid.Cells[i].GeoConn[j], SizeOf(Single));
        InStream.Read(SimData.Grid.Cells[i].TopoConn[j], SizeOf(Single));
        InStream.Read(SimData.Grid.Cells[i].RiversConn[j], SizeOf(Single));
       end;
     end;

    FreeAndNil(InStream);
  Except
    Exception.Create(#13#10 + #13#10 + #13#10 +
                     'Could not load the simulation data file!' + #13#10 +
                     'Press any key to continue...');
    ReadLn(i);
   End;

  Result:= True;
 end;

end.
