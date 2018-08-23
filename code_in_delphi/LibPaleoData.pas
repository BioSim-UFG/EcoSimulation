unit LibPaleoData;
{$mode delphi}{$H+}
interface

//bibliotecas / "includes"
uses
  SysUtils,
  Classes,
  ZLib,
  Math,
  LibTypes,   // biblioteca pessoal
  LibFiles,  // biblioteca pessoal
  LibText,    // biblioteca pessoal
  LibMatrix;  // biblioteca pessoal

Type
  PPaleoClimate = ^TPaleoClimate;
  TPaleoClimate = Class //declaração do nome da classe
    ModelGridnCells: Integer;             //variavel da classe TPaleoClimate
    PLASIMLats, PLASIMLongs: TSngVector;  //variavel da classe TPaleoClimate
    ModelGridLat, ModelGridLong: TSngVector;  //variavel da classe TPaleoClimate

    Type
      TLastInterpolation = Record   //Objeto-classe da classe TPaleoClimate
      PrevTimeStep, NextTimeStep: Integer;
      PrevSATMin, PrevSATMax, PrevPPTNMin, PrevPPTNMax, PrevNPP: Single;
      NextSATMin, NextSATMax, NextPPTNMin, NextPPTNMax, NextNPP: Single;
      End;


  Public
    // Interpolate only one cell, and get the four climatic variables for the cell
    Procedure GetClimCell(c: Integer;                                // Index of the model grid cell for which the interpolation will be calculated
                          TimeKya: Double;                           // Time for which the climate will be calculated. Measured in kya. Must be any continuous value between 5000 and 0
                                                                     // If Time is a round number, than the number comes straight from the time series of PLASIM
                                                                     // If Time has a decimal value, than it is temporally interpolated between two milenia
                          var SATMin,   
                              SATMax,
                              PPTNMin,
                              PPTNMax,
                              NPP: Single); overload;

    Procedure GetClimCell(c: Integer;   //passagem por valor
                          TimeStep: Integer;
                          var SATMin,   //passagem por referencia (var <nome> indica isso)
                              SATMax,
                              PPTNMin,
                              PPTNMax,
                              NPP: Single);  overload;

    // Interpolate the entire time series, for all cells of the model grid
    Procedure GetClimAtTime(TimeKya: Double;
                            var SATMin,
                                SATMax,
                                PPTNMin,
                                PPTNMax,
                                NPP: TSngVector); overload;

    Procedure GetClimAtTime(TimeStep: Integer;
                            var SATMin,
                                SATMax,
                                PPTNMin,
                                PPTNMax,
                                NPP: TSngVector); overload;

    Procedure GetClimGrid(TimeStep: Integer;
                          var EnvVec, NPPVec: PSngVector);

    // Interpolate the entire time series, for all cells of the model grid
    Procedure GetClimTimeSeries(StartTime, EndTime: Double;
                                TimeResolution: Double;
                                var SATMin,
                                    SATMax,
                                    PPTNMin,
                                    PPTNMax,
                                    NPP: TSngMatrix); overload;

    Procedure GetClimTimeSeries(TimeResolution: Double;
                                var SATMin,
                                    SATMax,
                                    PPTNMin,
                                    PPTNMax,
                                    NPP: TSngMatrix); overload;

    // Create the class by opening the PLASIM file and the present-day climatology data as baseline
    Constructor Create(PLASIMFile,
                       PresentClimateFile: String;
                       ProjectAnnomalies: Boolean = True);


  Private
    PLASIMnTime: Integer;
    PLASIMnTimeOffset: Integer;        // 1 if PLASIMnTime = 5001, and 10 if PLASIMnTime = 50001

    AdjLeft, AdjDown: Array of SmallInt; // PLASIM grid cells relating to model grid

    PLASIMDataSATMax, PLASIMDataSATMin: Array of TSngMatrix;
    PLASIMDataPPTNMax, PLASIMDataPPTNMin: Array of TSngMatrix;
    PLASIMDataNPP: Array of TSngMatrix;
    wLon, wLat: TSngVector;                // Weights for interpolation

    ModelGridObsClimate: TDblMatrix;       // Baseline/current climate from "observed" data (e.g. WorldClim)
    ModelGridPLASIMClimate: TSngMatrix;    // Predictions of PLASIM for the current climate

    LastInterpolation: Array of TLastInterpolation;
    ProjAnomalies: Boolean;

  End;  //fim da classe TPaleoClimate

implementation

Constructor TPaleoClimate.Create(PLASIMFile,
                                 PresentClimateFile: String;
                                 ProjectAnnomalies: Boolean = True);
 var
  PLASIMnLat, PLASIMnLong: Integer;
  Stream, ZipStream: TMemoryStream;

  i, j, k, c: Integer;
 begin

   ProjAnomalies:= ProjectAnnomalies;

   // Reads PLASIM Grid
   ZipStream:= TMemoryStream.Create;
   ZipStream.Position:= 0;
   ZipStream.LoadFromFile(PLASIMFile);
   Stream:= TMemoryStream.Create;
   ZDecompressStream(ZipStream, Stream);
   FreeAndNil(ZipStream);

   Stream.Position:= 0;







              //PLASIM é os dados climaticos
   Stream.Read(PLASIMnLong, SizeOf(Integer));
   SetLength(PLASIMLongs, PLASIMnLong);
   //lendo a longitude
   For i:= 0 to PLASIMnLong-1 do
     Stream.Read(PLASIMLongs[i], SizeOf(Single));

   Stream.Read(PLASIMnLat, SizeOf(Integer));
   SetLength(PLASIMLats, PLASIMnLat);
   //lendo a latitude
   For i:= 0 to PLASIMnLat-1 do
     Stream.Read(PLASIMLats[i], SizeOf(Single));

   // Original PLASIM data starts at 5.000.000 ya (5 million years ago, 5Mya), and may have different temporal resolutions (1ky or 100y)
   // For this class, user can either specify time as an integer (in that case, the time step), or actual time (in that case, kya)
   Stream.Read(PLASIMnTime, SizeOf(Integer));

   // PLASIMNTimeOffset = 1 if PLASIMnTime = 5001, or PLASIMNTimeOffset = 10 if PLASIMnTime = 50001
   PLASIMnTimeOffset:= Trunc((PLASIMnTime-1) / 5000);







   // Read PLASIM Minimum Temperature
   SetLength(PLASIMDataSATMin, PLASIMnLong, PLASIMnLat, PLASIMnTime);
   For i:= 0 to PLASIMnLong-1 do
    begin
     For j:= 0 to PLASIMnLat-1 do
      begin
       For k:= 0 to PLASIMnTime-1 do
        begin
         Stream.Read(PLASIMDataSATMin[i,j,k], SizeOf(Single));      //SATmin -> Surface Air Temperature Min ( minima do ano -> inverno)
        end;
      end;
    end;

   // Read PLASIM Maximum Temperature
   SetLength(PLASIMDataSATMax, PLASIMnLong, PLASIMnLat, PLASIMnTime);
   For i:= 0 to PLASIMnLong-1 do
    begin
     For j:= 0 to PLASIMnLat-1 do
      begin
       For k:= 0 to PLASIMnTime-1 do
        begin
         Stream.Read(PLASIMDataSATMax[i,j,k], SizeOf(Single));    //SATmax -> Surface Air Temperature Max ( maxima do ano -> verão)
        end;
      end;
    end;

   // Read PLASIM Minimum Precipitation
   SetLength(PLASIMDataPPTNMin, PLASIMnLong, PLASIMnLat, PLASIMnTime);
   For i:= 0 to PLASIMnLong-1 do
    begin
     For j:= 0 to PLASIMnLat-1 do
      begin
       For k:= 0 to PLASIMnTime-1 do
        begin
         Stream.Read(PLASIMDataPPTNMin[i,j,k], SizeOf(Single));         //PPTN PerciPiTatioN 
         PLASIMDataPPTNMin[i,j,k]:= (PLASIMDataPPTNMin[i,j,k] * 365) / 4;   // Convert mm/day to mm/season
        end;
      end;
    end;

   // Read PLASIM Maximum Precipitation
   SetLength(PLASIMDataPPTNMax, PLASIMnLong, PLASIMnLat, PLASIMnTime);
   For i:= 0 to PLASIMnLong-1 do
    begin
     For j:= 0 to PLASIMnLat-1 do
      begin
       For k:= 0 to PLASIMnTime-1 do
        begin
         Stream.Read(PLASIMDataPPTNMax[i,j,k], SizeOf(Single));
         PLASIMDataPPTNMax[i,j,k]:= (PLASIMDataPPTNMax[i,j,k] * 365) / 4;   // Convert mm/day to mm/season
        end;
      end;
    end;

   // Read PLASIM NPP
   SetLength(PLASIMDataNPP, PLASIMnLong, PLASIMnLat, PLASIMnTime);
   For i:= 0 to PLASIMnLong-1 do
    begin
     For j:= 0 to PLASIMnLat-1 do
      begin
       For k:= 0 to PLASIMnTime-1 do
        begin
         Stream.Read(PLASIMDataNPP[i,j,k], SizeOf(Single));     //Net Primary productivity (produtividade primaria líquida, basicamente a produtividade "ecologica", dependente da quantidade de luz e agua)
        end;
      end;
    end;

   FreeAndNil(Stream);




   // Open a file containing the coordinates of model grid, and "observed" present climate
   // The order of environmental variables must be: SAT_Min, SAT_Max, PPTN_Min, PPTN_Max

   ModelGridObsClimate:= OpenSimple(PresentClimateFile);

   ModelGridLong:= DoubleToSingle(Col(ModelGridObsClimate, 0));   //Col() -> obetem a coluna de uma matriz
   ModelGridObsClimate:= DelCol(ModelGridObsClimate, 0);

   ModelGridLat:= DoubleToSingle(Col(ModelGridObsClimate, 0));
   ModelGridObsClimate:= DelCol(ModelGridObsClimate, 0);


{// Ad hoc here!
   // Lock outliers of extreme rain at 1500mm/season
   For i:= 0 to Length(ModelGridObsClimate)-1 do
    begin
     If ModelGridObsClimate[i,2] > 1500 then
      begin
       ModelGridObsClimate[i,2]:= 1500;
      end;
     If ModelGridObsClimate[i,3] > 1500 then
      begin
       ModelGridObsClimate[i,3]:= 1500;
      end;
    end;
}


   ModelGridnCells:= Length(ModelGridObsClimate);


{ // For testing, input PLASIM's grid as the model grid, to get raw data
   k:= 0;
   For i:= 0 to Length(PLASIMLats)-1 do
    begin
     For j:= 0 to Length(PLASIMLongs)-1 do
      begin
       ModelGridLat[k]:= PLASIMLats[i];
       ModelGridLong[k]:= PLASIMLongs[j];
       k:= k + 1;
      end;
    end;
}

   // Create a map of references between Model grid cell and PLASIM cells
   SetLength(wLon, ModelGridnCells);
   SetLength(wLat, ModelGridnCells);
   SetLength(AdjLeft, ModelGridnCells);
   SetLength(AdjDown, ModelGridnCells);


   // Find the four cells whose centroids form the square, in which the modeled cell is contained.
   // The relative distance to each of those centroids will be used as weights for the interpolation


   // For each cell in the modeled grid
   For c:= 0 to ModelGridnCells-1 do
    begin
     // Start with a dummy/flag index
     AdjLeft[c]:= -1;

     // SEARCH THROUGH LONGITUDES FIRST, FIND THE INDEX OF THE ADJACENT COLUMN ON THE LEFT

     // Progresses through all columns of the map, from west to east, starting at lower longitudes (Alaska)
     // Finds the coordinate of the adjacent cell in the west
     For i:= 0 to PLASIMnLong-1 do
      begin
       // Is the current column of the PLASIM grid to the left of the modeled grid?
       If PLASIMLongs[i] < ModelGridLong[c] then
        begin
         // Records the column of the adjacent column to the left (west)
         // Keep updating for as long as it can find cells in the west, until it finds the adjacent column in the left
         AdjLeft[c]:= i;
        end

       // If the cell is in the east
       Else
        begin
         // and if we have already searched all columns on the left (west) cells
         If AdjLeft[c] <> -1 then
           // then there is no reason to keep searching, because all future columns will also be to the right (east)
           Break;
        end;
      end;

     // SEARCH THROUGH LATITUDES, FIND THE INDEX OF THE ADJACENT ROW BELOW

     // if the current cell has the same latitude as the previous one
     If (c > 0) and (ModelGridLat[c] = ModelGridLat[c-1]) then
      begin
       // then its neighbor to the south is also in the same row
       AdjDown[c]:= AdjDown[c-1];
      end
     Else
      begin
       // Start with a dummy/flag index
       AdjDown[c]:= 0;

       // Progresses through all rows (latitudes) of the original PLASIM grid
       // We will start in the north (positive latitudes), and progresses south towards negative latitudes
       For j:= 0 to PLASIMnLat-1 do
        begin
         // If this PLASIM row is immediatelly to the south, then
         If PLASIMLats[j] < ModelGridLat[c] then
          begin
           // Record the index of the row (latitude) immediatelly below the desired cell
           AdjDown[c]:= j;
           Break;
          end;
        end;
      end;

     // Pre-calculate the weights for interpolation of all model cells
     // Now that we have row and column of the adjacent cell in the left/bottom, we can calculate the weight given to the next row (right) and column (top)

     // Weight for western (right) PLASIM cell
     // Weight for the easten (left) PLASIM cell is (1 - western)
     wLon[c]:= (PLASIMLongs[AdjLeft[c]+1] - ModelGridLong[c]) / (PLASIMLongs[AdjLeft[c]+1] - PLASIMLongs[AdjLeft[c]]);
     If (wLon[c] > 1) or (wLon[c] < 0) then
       wLon[c]:= 0;

     // weight for southern (bottom) PLASIM cell
     // Weight for the northern (top) PLASIM cell is (1 - northern)
     wLat[c]:= (PLASIMLats[AdjDown[c]-1]  - ModelGridLat[c]) / (PLASIMLats[AdjDown[c]-1]  - PLASIMLats[AdjDown[c]]);
     If (wLat[c] > 1) or (wLat[c] < 0) then
       wLat[c]:= 0;
    end;

   If ProjAnomalies then
    begin
     // A first call to Interpolate is necessary to calculate the PLASIM prediction of climate for the current time
     // This PLASIM prediction to the present is necessary to set a baseline for the computation of the anomalies
     SetLength(ModelGridPLASIMClimate, ModelGridnCells, 5);

     For c:= 0 to ModelGridnCells-1 do
      begin
       GetClimCell(c, Integer(-1),                  // Yes, the time is negative because this is a first call
                   ModelGridPLASIMClimate[c,0],     // Yes, the matrix is CurrentPLASIM because this is a first call
                   ModelGridPLASIMClimate[c,1],
                   ModelGridPLASIMClimate[c,2],
                   ModelGridPLASIMClimate[c,3],
                   ModelGridPLASIMClimate[c,4]);
      end;
    end;
 end;

Procedure TPaleoClimate.GetClimCell(c: Integer; TimeKya: Double; var SATMin, SATMax, PPTNMin, PPTNMax, NPP: Single);
 var
  TimeInt, i: Integer;
  TimeIntPrev, TimeIntNext: Integer;
  DistPrev, DistNext: Double;
  Tmp: Double;
 begin
  // A negative time should not happen
  // The only exception is the creation of the classe, when the raw PLASIM estimate of climate in the last time step is performed in the model grid
  // That estimate is used to later plug PLASIM annomalies into the current "observe" climate data
  If TimeKya < 0 then
   begin
    TimeInt:= -1;
    GetClimCell(c, TimeInt, SATMin, SATMax, PPTNMin, PPTNMax, NPP);
   end

  Else
   begin
    Tmp:= ((TimeKya * PLASIMNTimeOffset) - Trunc(TimeKya * PLASIMNTimeOffset));
    Tmp:= RoundTo(Tmp, -4);

    // User has requested the estimated climate at an exact millenia.
    // There is no need for temporal interpolation of PLASIM data, just spatial interpolation
    If Tmp < 0.001 then
     begin
      // This is the time step that we want
      TimeInt:= Trunc((5000 - TimeKya) * PLASIMNTimeOffset);

      // Maybe the user has done some previous interpolation that may be useful here...
      If LastInterpolation <> Nil then
       begin
        // Previous time step?
        If TimeInt = LastInterpolation[c].PrevTimeStep then
         begin
          SATMin:= LastInterpolation[c].PrevSATMin;
          SATMax:= LastInterpolation[c].PrevSATMax;
          PPTNMin:= LastInterpolation[c].PrevPPTNMin;
          PPTNMax:= LastInterpolation[c].PrevPPTNMax;
          NPP:= LastInterpolation[c].PrevNPP;
         end Else

        // Next time step?
        If TimeInt = LastInterpolation[c].NextTimeStep then
         begin
          SATMin:= LastInterpolation[c].NextSATMin;
          SATMax:= LastInterpolation[c].NextSATMax;
          PPTNMin:= LastInterpolation[c].NextPPTNMin;
          PPTNMax:= LastInterpolation[c].NextPPTNMax;
          NPP:= LastInterpolation[c].NextNPP;
         end

        // Yes... we have to calculate it...
        Else
         begin
          GetClimCell(c, TimeInt, SATMin, SATMax, PPTNMin, PPTNMax, NPP);
         end;
       end
      Else
       begin
        GetClimCell(c, TimeInt, SATMin, SATMax, PPTNMin, PPTNMax, NPP);
       end;
     end

    // User has requested an estime of time in between millenia.
    // In this case, one must temporally interpolate between two time-slices of PLASIM data
    Else
     begin
      // Has the user done some interpolation yet?
      // If not, then create some space to save and later recycle the estimates
      If LastInterpolation = Nil then
       begin
        SetLength(LastInterpolation, ModelGridnCells);
        For i:= 0 to ModelGridnCells-1 do
         begin
          LastInterpolation[i].PrevTimeStep:= -1;
          LastInterpolation[i].NextTimeStep:= -1;
         end;
       end;

      // Climate in the last time step before the request
      TimeIntPrev:= 5000 - (Trunc(TimeKya * PLASIMNTimeOffset) + 1);

      // Climate in the time step immediately after the request
      TimeIntNext:= 5000 - Trunc(TimeKya * PLASIMNTimeOffset);

      // is the climate estimates that we need already computed?
      If (TimeIntPrev = LastInterpolation[c].PrevTimeStep) then
       begin
        // Do nothing... as we already have computed these values...
       end else

      // Is it computed as future time step, and now that the user has changed the time slice it is past?
      If (TimeIntPrev = LastInterpolation[c].NextTimeStep) then
       begin
        // Transfer the data from the future to the past
        LastInterpolation[c].PrevTimeStep:= LastInterpolation[c].NextTimeStep;
        LastInterpolation[c].PrevSATMin:= LastInterpolation[c].NextSATMin;
        LastInterpolation[c].PrevSATMax:= LastInterpolation[c].NextSATMax;
        LastInterpolation[c].PrevPPTNMin:= LastInterpolation[c].NextPPTNMin;
        LastInterpolation[c].PrevPPTNMax:= LastInterpolation[c].NextPPTNMax;
        LastInterpolation[c].PrevNPP:= LastInterpolation[c].NextNPP;
       end
      Else
       begin
        // We need to re-estimate the climate for both time steps, as they have not yet been computed

        // Saves the estimated climate in the two adjacent time steps,
        // in case the user wants to interpolate again within the interval
        LastInterpolation[c].PrevTimeStep:= TimeIntPrev;
        GetClimCell(c, TimeIntPrev,
                    LastInterpolation[c].PrevSATMin,
                    LastInterpolation[c].PrevSATMax,
                    LastInterpolation[c].PrevPPTNMin,
                    LastInterpolation[c].PrevPPTNMax,
                    LastInterpolation[c].PrevNPP);
       end;

      If (TimeIntNext = LastInterpolation[c].NextTimeStep) then
       begin
        // Do nothing here...
       end Else

      If (TimeIntNext = LastInterpolation[c].PrevTimeStep) then
       begin
        // Transfer the data from the future to the past
        LastInterpolation[c].NextTimeStep:= LastInterpolation[c].PrevTimeStep;
        LastInterpolation[c].NextSATMin:= LastInterpolation[c].PrevSATMin;
        LastInterpolation[c].NextSATMax:= LastInterpolation[c].PrevSATMax;
        LastInterpolation[c].NextPPTNMin:= LastInterpolation[c].PrevPPTNMin;
        LastInterpolation[c].NextPPTNMax:= LastInterpolation[c].PrevPPTNMax;
        LastInterpolation[c].NextNPP:= LastInterpolation[c].PrevNPP;
       end

      Else
       begin
        // We need to re-estimate the climate for both time steps...
        LastInterpolation[c].NextTimeStep:= TimeIntNext;
        GetClimCell(c, TimeIntNext,
                    LastInterpolation[c].NextSATMin,
                    LastInterpolation[c].NextSATMax,
                    LastInterpolation[c].NextPPTNMin,
                    LastInterpolation[c].NextPPTNMax,
                    LastInterpolation[c].NextNPP);
       end;

      DistNext:= TimeKya - Trunc(TimeKya);
      DistPrev:= 1 - DistNext;

      SATMin:=  (LastInterpolation[c].PrevSATMin * DistPrev)  + (LastInterpolation[c].NextSATMin * DistNext);
      SATMax:=  (LastInterpolation[c].PrevSATMax * DistPrev)  + (LastInterpolation[c].NextSATMax * DistNext);
      PPTNMin:= (LastInterpolation[c].PrevPPTNMin * DistPrev) + (LastInterpolation[c].NextPPTNMin * DistNext);
      PPTNMax:= (LastInterpolation[c].PrevPPTNMax * DistPrev) + (LastInterpolation[c].NextPPTNMax * DistNext);
      NPP:= (LastInterpolation[c].PrevNPP * DistPrev) + (LastInterpolation[c].NextNPP * DistNext);
     end;
   end;
 end;

Procedure TPaleoClimate.GetClimCell(c: Integer; TimeStep: Integer; var SATMin, SATMax, PPTNMin, PPTNMax, NPP: Single);
 var
  t: Integer;
  Tmp1, Tmp2: Double;
  FirstInterpolation: Boolean;
 begin

// With the definition of the four adjacent cells, whose centroids form the square, in which the modeled cell is contained...
// And having calculated the relative distance to each of those centroids, to be used as weights for the interpolation...
// We now calculate the interpolated value for the cell, for a given time step

   // A regular call to the interpolate function
   If TimeStep >= 0 then
    begin
     t:= Trunc(TimeStep);
     FirstInterpolation:= False;
    end

   // A special call to the interpolation function, to set up the prediction of present climate by PLASIM in the model grid
   Else
    begin
     t:= PLASIMnTime-1;
     FirstInterpolation:= True;
    end;

   // First interpolate the absolute PLASIM climate on the model grid

   If (AdjLeft[c] >= 63) or (AdjDown[c] <= 0) then
    begin
     SATMax:= NaN;
     SATMin:= NaN;
     PPTNMax:= NaN;
     PPTNMin:= NaN;
     NPP:= NaN;

     Exit;
    end
   Else
    begin
     // If there are no PLASIM cells to the left
     // So consider only the value at the cell to the right
     If (wLon[c] = 0) then
      begin
       Tmp1:= PLASIMDataSATMax[AdjLeft[c]+1, AdjDown[c],   t]; // cell at the south                       // ModelGridLat[c]
       Tmp2:= PLASIMDataSATMax[AdjLeft[c]+1, AdjDown[c]-1, t]; // cell at the north                       // ModelGridLong[c]
      end Else

     // If there are no cells to the right
     // consider only the cell to the left
     If (wLon[c] = 1) then
      begin
       Tmp1:= PLASIMDataSATMax[AdjLeft[c],   AdjDown[c],   t]; // cell at the south
       Tmp2:= PLASIMDataSATMax[AdjLeft[c],   AdjDown[c]-1, t]; // cell at the north
      end

     // If there are both left and right cells
     Else
      begin
       // The southern border
       Tmp1:= PLASIMDataSATMax[AdjLeft[c],   AdjDown[c],   t] *      wLon[c] +   // the eastern border       PLASIMLongs[AdjLeft[c]]      PLASIMLats[AdjDown[c]]
              PLASIMDataSATMax[AdjLeft[c]+1, AdjDown[c],   t] * (1 - wLon[c]);   // the western border       PLASIMLongs[AdjLeft[c]+1]    PLASIMLats[AdjDown[c]]

       // the northern border
       Tmp2:= PLASIMDataSATMax[AdjLeft[c],   AdjDown[c]-1, t] *      wLon[c] +   // the eastern border       PLASIMLongs[AdjLeft[c]]      PLASIMLats[AdjDown[c]-1]
              PLASIMDataSATMax[AdjLeft[c]+1, AdjDown[c]-1, t] * (1 - wLon[c]);   // the western border       PLASIMLongs[AdjLeft[c]+1]    PLASIMLats[AdjDown[c]-1]
      end;

     SATMax:= Tmp1 * wLat[c] +                        // the southern border
              Tmp2 * (1 - wLat[c]);                   // the northern border


     // If there are no cells to the right
     If (wLon[c] = 0) then
      begin
       Tmp1:= PLASIMDataSATMin[AdjLeft[c]+1, AdjDown[c],   t];
       Tmp2:= PLASIMDataSATMin[AdjLeft[c]+1, AdjDown[c]-1, t];
      end Else

     // If there are no cells to the left
     If (wLon[c] = 1) then
      begin
       Tmp1:= PLASIMDataSATMin[AdjLeft[c],   AdjDown[c],   t];
       Tmp2:= PLASIMDataSATMin[AdjLeft[c],   AdjDown[c]-1, t];
      end

     // If there are both left and right cells
     Else
      begin
       Tmp1:= PLASIMDataSATMin[AdjLeft[c],   AdjDown[c],   t] *      wLon[c] +
              PLASIMDataSATMin[AdjLeft[c]+1, AdjDown[c],   t] * (1 - wLon[c]);

       Tmp2:= PLASIMDataSATMin[AdjLeft[c],   AdjDown[c]-1, t] *      wLon[c] +
              PLASIMDataSATMin[AdjLeft[c]+1, AdjDown[c]-1, t] * (1 - wLon[c]);
      end;

     SATMin:= Tmp1 * wLat[c] +
              Tmp2 * (1 - wLat[c]);


     // If there are no cells to the right
     If (wLon[c] = 0) then
      begin
       Tmp1:= PLASIMDataPPTNMax[AdjLeft[c]+1, AdjDown[c],   t];
       Tmp2:= PLASIMDataPPTNMax[AdjLeft[c]+1, AdjDown[c]-1, t];
      end Else

     // If there are no cells to the left
     If (wLon[c] = 1) then
      begin
       Tmp1:= PLASIMDataPPTNMax[AdjLeft[c],   AdjDown[c],   t];
       Tmp2:= PLASIMDataPPTNMax[AdjLeft[c],   AdjDown[c]-1, t];
      end

     // If there are both left and right cells
     Else
      begin
       Tmp1:= PLASIMDataPPTNMax[AdjLeft[c],   AdjDown[c],   t] *      wLon[c] +   // the eastern border       PLASIMLongs[AdjLeft[c]]      PLASIMLats[AdjDown[c]]
              PLASIMDataPPTNMax[AdjLeft[c]+1, AdjDown[c],   t] * (1 - wLon[c]);   // the western border       PLASIMLongs[AdjLeft[c]+1]    PLASIMLats[AdjDown[c]]

       // the northern border
       Tmp2:= PLASIMDataPPTNMax[AdjLeft[c],   AdjDown[c]-1, t] *      wLon[c] +   // the eastern border       PLASIMLongs[AdjLeft[c]]      PLASIMLats[AdjDown[c]-1]
              PLASIMDataPPTNMax[AdjLeft[c]+1, AdjDown[c]-1, t] * (1 - wLon[c]);   // the western border       PLASIMLongs[AdjLeft[c]+1]    PLASIMLats[AdjDown[c]-1]
      end;

     PPTNMax:= Tmp1 * wLat[c] +                        // the southern border
               Tmp2 * (1 - wLat[c]);                   // the northern border


     // If there are no cells to the right
     If (wLon[c] = 0) then
      begin
       Tmp1:= PLASIMDataPPTNMin[AdjLeft[c]+1, AdjDown[c],   t];
       Tmp2:= PLASIMDataPPTNMin[AdjLeft[c]+1, AdjDown[c]-1, t];
      end Else

     // If there are no cells to the left
     If (wLon[c] = 1) then
      begin
       Tmp1:= PLASIMDataPPTNMin[AdjLeft[c],   AdjDown[c],   t];
       Tmp2:= PLASIMDataPPTNMin[AdjLeft[c],   AdjDown[c]-1, t];
      end

     // If there are both left and right cells
     Else
      begin
       Tmp1:= PLASIMDataPPTNMin[AdjLeft[c],   AdjDown[c],   t] *      wLon[c] +
              PLASIMDataPPTNMin[AdjLeft[c]+1, AdjDown[c],   t] * (1 - wLon[c]);

       Tmp2:= PLASIMDataPPTNMin[AdjLeft[c],   AdjDown[c]-1, t] *      wLon[c] +
              PLASIMDataPPTNMin[AdjLeft[c]+1, AdjDown[c]-1, t] * (1 - wLon[c]);
      end;

     PPTNMin:= Tmp1 * wLat[c] +
               Tmp2 * (1 - wLat[c]);

     // Zero Precipitation value if the emulated precipitation was negative
     If PPTNMax < 0 then
       PPTNMax:= 0.04;    // minimum observed value in current climatology
     If PPTNMin < 0 then
       PPTNMin:= 0;




     // If there are no cells to the right
     If (wLon[c] = 0) then
      begin
       Tmp1:= PLASIMDataNPP[AdjLeft[c]+1, AdjDown[c],   t];
       Tmp2:= PLASIMDataNPP[AdjLeft[c]+1, AdjDown[c]-1, t];
      end Else

     // If there are no cells to the left
     If (wLon[c] = 1) then
      begin
       Tmp1:= PLASIMDataNPP[AdjLeft[c],   AdjDown[c],   t];
       Tmp2:= PLASIMDataNPP[AdjLeft[c],   AdjDown[c]-1, t];
      end

     // If there are both left and right cells
     Else
      begin
       Tmp1:= PLASIMDataNPP[AdjLeft[c],   AdjDown[c],   t] *      wLon[c] +
              PLASIMDataNPP[AdjLeft[c]+1, AdjDown[c],   t] * (1 - wLon[c]);

       Tmp2:= PLASIMDataNPP[AdjLeft[c],   AdjDown[c]-1, t] *      wLon[c] +
              PLASIMDataNPP[AdjLeft[c]+1, AdjDown[c]-1, t] * (1 - wLon[c]);
      end;

     NPP:= Tmp1 * wLat[c] +
           Tmp2 * (1 - wLat[c]);

     If NPP < 0 then
       NPP:= 0;
    end;


   // A first call to Interpolate is necessary to calculate the raw PLASIM prediction of climate at present time
   // This PLASIM prediction is later used as a baseline for the computation of the anomalies in relation to the present observed climate (i.e. worldclim)
   // Because the code below is the application of annomalies in relation to the present observed climate, and here we need the raw PLASIm prediction, we may just exit here
   // If, instead, we want to interpolate the raw PLASIM data for all time steps, then we can just disable the "Exit" also calculate the "annomalies" during the creation of the class
   If FirstInterpolation or (not ProjAnomalies) then
    begin
     Exit;
    end;


// Convert anomalies from current climate
   // Temperature
   // Additive anomalies for temperature
   SATMin:= SATMin -               // Current time step
            ModelGridPLASIMClimate[c,0] +   // Last time step
            ModelGridObsClimate[c,0];   // Current climatology (sat_min, sat_max, pptn_min, pptn_max)

   SATMax:= SATMax -
            ModelGridPLASIMClimate[c,1] +
            ModelGridObsClimate[c,1];

   // If SATMax is less than SATMin, then calculate the average
   If SATMax < SATMin then
    begin
     SATMax:= (SATMax + SATMin) / 2;
     SATMin:= SATMax;
    end;


   // Multiplicative anomalies for precipitation
   If ModelGridPLASIMClimate[c,3] > 0 then
    begin
     If ModelGridObsClimate[c,3] <= ModelGridPLASIMClimate[c,3] then
      begin
       PPTNMax:= (PPTNMax /
                  ModelGridPLASIMClimate[c,3]) *
                  ModelGridObsClimate[c,3];
      end
     Else
      begin
       PPTNMax:= PPTNMax -
                 ModelGridPLASIMClimate[c,3] +
                 ModelGridObsClimate[c,3];
      end;
    end
   Else
    begin
     PPTNMax:= 0;
    end;

   If ModelGridPLASIMClimate[c,2] > 0 then
    begin
     If ModelGridObsClimate[c,2] <= ModelGridPLASIMClimate[c,2] then
      begin
       PPTNMin:= (PPTNMin /
                  ModelGridPLASIMClimate[c,2]) *
                  ModelGridObsClimate[c,2];
      end
     Else
      begin
       PPTNMin:= PPTNMin -
                 ModelGridPLASIMClimate[c,2] +
                 ModelGridObsClimate[c,2];
      end;
    end
   Else
    begin
     PPTNMin:= 0;
    end;


   // If PPTNMax is less than PPTNMin, then calculate the average
   If PPTNMax < PPTNMin then
    begin
     PPTNMax:= (PPTNMax + PPTNMin) / 2;
     PPTNMin:= PPTNMax;
    end;




// For testing purposes one may decide to plot the raw climatology or emulated data
{
  SATMin:= ModelGridPLASIMClimate[c,0];
  SATMax:= ModelGridPLASIMClimate[c,1];
  PPTNMin:= ModelGridPLASIMClimate[c,2];
  PPTNMax:= ModelGridPLASIMClimate[c,3];
}
{
  SATMin:= ModelGridObsClimate[c,0];
  SATMax:= ModelGridObsClimate[c,1];
  PPTNMin:= ModelGridObsClimate[c,2];
  PPTNMax:= ModelGridObsClimate[c,3];
}
{
  SATMin:= ModelGridPLASIMClimate[c,0] - ModelGridObsClimate[c,0];
  SATMax:= ModelGridPLASIMClimate[c,1] - ModelGridObsClimate[c,1];
  If SATMax > +20 then
    SATMax:= + 20;
  If SATMax < -20 then
    SATMax:= -20;
  If SATMin > +20 then
    SATMin:= + 20;
  If SATMin < -20 then
    SATMin:= -20;

  PPTNMin:= ModelGridPLASIMClimate[c,2] - ModelGridObsClimate[c,2];
  PPTNMax:= ModelGridPLASIMClimate[c,3] - ModelGridObsClimate[c,3];
  If PPTNMax > +2000 then
    PPTNMax:= + 2000;
  If PPTNMax < -2000 then
    PPTNMax:= -2000;
  If PPTNMin > +2000 then
    PPTNMin:= + 2000;
  If PPTNMin < -2000 then
    PPTNMin:= -2000;
}




//{ // Capping PPTN at 2000mm / season
  If PPTNMin > 2000 then
    PPTNMin:= 2000;

  If PPTNMax > 2000 then
    PPTNMax:= 2000;
//}

   // Multiplicative anomalies for NPP
   If not IsNaN(ModelGridObsClimate[c,4]) then
    begin
     If ModelGridPLASIMClimate[c,4] > 0 then
      begin
       NPP:= (NPP /
              ModelGridPLASIMClimate[c,4]) *
              ModelGridObsClimate[c,4];
      end
     Else
      begin
       NPP:= 0;
      end;

     If NPP < 0 then
      begin
       NPP:= 0;
      end;
    end
   Else
    begin
     NPP:= NaN;
    end;
 end;

// Interpolate the entire time series, for all cells of the model grid
Procedure TPaleoClimate.GetClimAtTime(TimeKya: Double;
                                      var SATMin,
                                          SATMax,
                                          PPTNMin,
                                          PPTNMax,
                                          NPP: TSngVector);
 var
  c: Integer;
 begin
  If Length(SATMax) <> ModelGridnCells then
    SetLength(SATMax, ModelGridnCells);

  If Length(SATMin) <> ModelGridnCells then
    SetLength(SATMin, ModelGridnCells);

  If Length(PPTNMax) <> ModelGridnCells then
    SetLength(PPTNMax, ModelGridnCells);

  If Length(PPTNMin) <> ModelGridnCells then
    SetLength(PPTNMin, ModelGridnCells);

  If Length(NPP) <> ModelGridnCells then
    SetLength(NPP, ModelGridnCells);

  For c:= 0 to ModelGridnCells-1 do
   begin
    GetClimCell(c, TimeKya,
                SATMin[c],
                SATMax[c],
                PPTNMin[c],
                PPTNMax[c],
                NPP[c]);
   end;
 end;

Procedure TPaleoClimate.GetClimAtTime(TimeStep: Integer;
                                      var SATMin,
                                          SATMax,
                                          PPTNMin,
                                          PPTNMax,
                                          NPP: TSngVector);
 var
  c: Integer;
 begin
  If Length(SATMax) <> ModelGridnCells then
    SetLength(SATMax, ModelGridnCells);

  If Length(SATMin) <> ModelGridnCells then
    SetLength(SATMin, ModelGridnCells);

  If Length(PPTNMax) <> ModelGridnCells then
    SetLength(PPTNMax, ModelGridnCells);

  If Length(PPTNMin) <> ModelGridnCells then
    SetLength(PPTNMin, ModelGridnCells);

  If Length(NPP) <> ModelGridnCells then
    SetLength(NPP, ModelGridnCells);

  For c:= 0 to ModelGridnCells-1 do
   begin
    GetClimCell(c, TimeStep,
                SATMin[c],
                SATMax[c],
                PPTNMin[c],
                PPTNMax[c],
                NPP[c]);
   end;
 end;

// EnvVec is a single array containing four environmental variables for the entire grid
// Thus, the length of EnvVec is four times the number of grid cells
// The order is regular: For each grid cell, SATMin, SATMax, PPTNMin, PPTN
Procedure TPaleoClimate.GetClimGrid(TimeStep: Integer;
                                    var EnvVec, NPPVec: PSngVector);
 var
  TmpEnvVec, TmpNPPVec: TSngVector;
 begin
  TmpEnvVec:= EnvVec^;
  TmpNPPVec:= NPPVec^;

  If (Length(TmpEnvVec) div 4) <> ModelGridnCells then
    SetLength(TmpEnvVec, ModelGridnCells * 4);

  If Length(TmpNPPVec) <> ModelGridnCells then
    SetLength(TmpNPPVec, ModelGridnCells);

  TParallel.For(0, ModelGridnCells-1,
    Procedure (c: Integer)
     begin
      GetClimCell(c, TimeStep,
                  TmpEnvVec[(c*4) + 0],
                  TmpEnvVec[(c*4) + 1],
                  TmpEnvVec[(c*4) + 2],
                  TmpEnvVec[(c*4) + 3],
                  TmpNPPVec[c]);
     end);
 end;

Procedure TPaleoClimate.GetClimTimeSeries(StartTime, EndTime: Double;
                                          TimeResolution: Double;
                                          var SATMin,
                                              SATMax,
                                              PPTNMin,
                                              PPTNMax,
                                              NPP: TSngMatrix);
 var
  k, c, nSteps: Integer;
  t, tSpan: Double;
 begin
   tSpan:= StartTime - EndTime;
   nSteps:= Trunc(tSpan / TimeResolution) + 1;

   SetLength(SATMax, ModelGridnCells, nSteps);
   SetLength(SATMin, ModelGridnCells, nSteps);
   SetLength(PPTNMax, ModelGridnCells, nSteps);
   SetLength(PPTNMin, ModelGridnCells, nSteps);
   SetLength(NPP, ModelGridnCells, nSteps);

   t:= StartTime;
   For k:= 0 to nSteps-1 do
    begin
     For c:= 0 to ModelGridnCells-1 do
      begin
       GetClimCell(c, t,
                   SATMin[c,k],
                   SATMax[c,k],
                   PPTNMin[c,k],
                   PPTNMax[c,k],
                   NPP[c,k]);
      end;

     t:= t - TimeResolution;
    end;
 end;

Procedure TPaleoClimate.GetClimTimeSeries(TimeResolution: Double;
                                          var SATMin,
                                              SATMax,
                                              PPTNMin,
                                              PPTNMax,
                                              NPP: TSngMatrix);
 begin
   GetClimTimeSeries(5000,
                     0,
                     TimeResolution,
                     SATMin,
                     SATMax,
                     PPTNMin,
                     PPTNMax,
                     NPP);
 end;
end.
