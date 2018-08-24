program PaleoData;

uses
  SysUtils,
  LibPaleoData in '..\..\Libs\LibPaleoData.pas';

var
  PaleoClimate: TPaleoClimate;

  PaleoClimFile: String;
  PresentClimFile: String;
  r: String;

  OutSuffix: String;
  OutMinTemp, OutMaxTemp, OutMinPPTN, OutMaxPPTN, OutNPP: TextFile;

  StartTime, EndTime, StepTime: Double;

  c, v, t, nt: Integer;
  b, e, s: Double;

  SATMin, SATMax, PPTNMin, PPTNMax, NPP: Single;

begin
  try
   WriteLn('Is the grid + present climate file ''Coords and Clim.txt''?');
   WriteLn('This file should as many rows as there are grid cells, and 7 columns:');
   WriteLn('1) Longitude');
   WriteLn('2) Latitude');
   WriteLn('3) Temperature in the coldest quarter');
   WriteLn('4) Temperature in the warmest quarter');
   WriteLn('5) Precipitation in the driest quarter');
   WriteLn('6) Precipitation in the wettest quarter');
   WriteLn('7) Annual NPP');
   WriteLn('');
   WriteLn('Type ENTER or "Y" if yes, or type the correct name of the paleoclimate file');
   r:= '';
   ReadLn(r);

   If ('Y' = uppercase(r)) or ('YES' = uppercase(r)) or (r = '') then
    begin
     PresentClimFile:= 'Coords and Clim.txt';
    end
   Else
    begin
     PresentClimFile:= r;
    end;






   WriteLn('');
   WriteLn('');
   WriteLn('');
   WriteLn('');
   WriteLn('Is the paleoclimate file ''xPLASIM.All3DMats.Single.Zip.Stream''?');
   WriteLn('Type ENTER or "Y" if yes, or the correct name of the paleoclimate file');
   r:= '';
   ReadLn(r);

   If ('Y' = uppercase(r)) or ('YES' = uppercase(r)) or (r = '') then
    begin
     PaleoClimFile:= 'xPLASIM.All3DMats.Single.Zip.Stream';
    end
   Else
    begin
     PaleoClimFile:= r;
    end;





   WriteLn('');
   WriteLn('');
   WriteLn('');
   WriteLn('');
   WriteLn('Would you like to use the suffix ''!OutPaleoClim - Var.txt*''?');
   WriteLn('');
   WriteLn('Type ENTER or "Y" if yes, or type the suffix for the output files');
   WriteLn('If you would like to customize the suffix, enter only a single word (e.g. ''ClimDat'')');
   r:= '';
   ReadLn(r);

   If ('Y' = uppercase(r)) or ('YES' = uppercase(r)) or (r = '') then
    begin
     OutSuffix:= '!OutPaleoClim';
    end
   Else
    begin
     OutSuffix:= r;
    end;





   WriteLn('');
   WriteLn('');
   WriteLn('');
   WriteLn('');
   WriteLn('Would you like to start 5M years ago?');
   WriteLn('');
   WriteLn('Type ENTER or "Y" if yes, or enter the starting time in thousand of years ago (5000 = 5Mya)');
   r:= '';
   ReadLn(r);

   If ('Y' = uppercase(r)) or ('YES' = uppercase(r)) or (r = '') then
    begin
     b:= 5000;
    end
   Else
    begin
     b:= StrToFloat(r);
    end;






   WriteLn('');
   WriteLn('');
   WriteLn('');
   WriteLn('');
   WriteLn('Would you like to end in the present time?');
   WriteLn('');
   WriteLn('Type ENTER or "Y" if yes, or enter the ending time in thousand of years ago (1000 = 1Mya)');
   r:= '';
   ReadLn(r);

   If ('Y' = uppercase(r)) or ('YES' = uppercase(r)) or (r = '') then
    begin
     e:= 0;
    end
   Else
    begin
     e:= StrToFloat(r);
    end;








   WriteLn('');
   WriteLn('');
   WriteLn('');
   WriteLn('');
   WriteLn('Would you like to use 500 years as time step?');
   WriteLn('');
   WriteLn('Type ENTER or "Y" if yes, or enter the length of time step in thousand of years ago (0.5 = 500y)');
   r:= '';
   ReadLn(r);

   If ('Y' = uppercase(r)) or ('YES' = uppercase(r)) or (r = '') then
    begin
     s:= 0.5;
    end
   Else
    begin
     s:= StrToFloat(r);
    end;


   PaleoClimate:= TPaleoClimate.Create(PaleoClimFile,
                                       PresentClimFile,
                                       True);



   AssignFile(OutMinTemp, OutSuffix + ' - MinTemp.txt');
   AssignFile(OutMaxTemp, OutSuffix + ' - MaxTemp.txt');
   AssignFile(OutMinPPTN, OutSuffix + ' - MinPPTN.txt');
   AssignFile(OutMaxPPTN, OutSuffix + ' - MaxPPTN.txt');
   AssignFile(OutNPP,     OutSuffix + ' - NPP.txt');
   ReWrite(OutMinTemp);
   ReWrite(OutMaxTemp);
   ReWrite(OutMinPPTN);
   ReWrite(OutMaxPPTN);
   ReWrite(OutNPP);

   nt:= Trunc((b - e) / s);

   Write(OutMinTemp, 'T' + Float2Str(b,1));
   Write(OutMaxTemp, 'T' + Float2Str(b,1));
   Write(OutMinPPTN, 'T' + Float2Str(b,1));
   Write(OutMaxPPTN, 'T' + Float2Str(b,1));
   Write(OutNPP, 'T' + Float2Str(b,1));
   for t:= 1 to nt do
    begin
     Write(OutMinTemp, #9, 'T' + Float2Str((b-(t*s)),1));
     Write(OutMaxTemp, #9, 'T' + Float2Str((b-(t*s)),1));
     Write(OutMinPPTN, #9, 'T' + Float2Str((b-(t*s)),1));
     Write(OutMaxPPTN, #9, 'T' + Float2Str((b-(t*s)),1));
     Write(OutNPP, #9, 'T' + Float2Str((b-(t*s)),1));
    end;
   WriteLn(OutMinTemp, '');
   WriteLn(OutMaxTemp, '');
   WriteLn(OutMinPPTN, '');
   WriteLn(OutMaxPPTN, '');
   WriteLn(OutNPP, '');

   For c:= 0 to Length(PaleoClimate.ModelGridLat)-1 do
    begin
     PaleoClimate.GetClimCell(c, b, SATMin, SATMax, PPTNMin, PPTNMax, NPP);

     Write(OutMinTemp, SATMin:0:3);
     Write(OutMaxTemp, SATMax:0:3);
     Write(OutMinPPTN, PPTNMin:0:3);
     Write(OutMaxPPTN, PPTNMax:0:3);
     Write(OutNPP, NPP:0:3);

     for t:= 1 to nt do
      begin
       PaleoClimate.GetClimCell(c, (b-(t*s)), SATMin, SATMax, PPTNMin, PPTNMax, NPP);
       Write(OutMinTemp, #9, SATMin:0:3);
       Write(OutMaxTemp, #9, SATMax:0:3);
       Write(OutMinPPTN, #9, PPTNMin:0:3);
       Write(OutMaxPPTN, #9, PPTNMax:0:3);
       Write(OutNPP, #9, NPP:0:3);
      end;

     WriteLn(OutMinTemp, '');
     WriteLn(OutMaxTemp, '');
     WriteLn(OutMinPPTN, '');
     WriteLn(OutMaxPPTN, '');
     WriteLn(OutNPP, '');
    end;

   CloseFile(OutMinTemp);
   CloseFile(OutMaxTemp);
   CloseFile(OutMinPPTN);
   CloseFile(OutMaxPPTN);
   CloseFile(OutNPP);

  except
    on E: Exception do
      Writeln(E.ClassName, ': ', E.Message);
  end;
end.
