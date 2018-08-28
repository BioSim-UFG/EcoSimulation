unit LibTetraCells;

interface

Uses
  System.SysUtils,
  System.Classes,
  System.Threading,
  Generics.Collections,
  Math,

  {$UnDef DebugPerformance}
  {$IfDef DebugPerformance}
    Windows,
  {$EndIf}

  LibTypes,
  LibGeometry,
  LibPolyClipper,
  LibProbDistrib,
  LibPaleoData;

Type
  TEnvValue = Record
    Maximum: PSingle;                     // Annual maxima per cell, in a given time, for a given variable
    Center: Single;                      // Midpoint between mininum and maximum
    Minimum: PSingle;                     // Annual minima per cell, In a given time for a given variable
   End;
  TEnvValues = Array of TEnvValue;

  TNicheValue = Record
    Maximum: Single;
    Center: Single;
    Minimum: Single;
   End;
  TNicheValues = Array of TNicheValue;

  PClimate = ^TClimate;
  TClimate = Record
    EnvVars: TEnvValues;                 // Temperature and precipitation
    NPP: PSingle;                         // NPP
   End;

  PMapCell = ^TMapCell;
  TMapCell = Record
    IdCell: Integer;                     // Unique Id of a each cell, which is its index in the list of cells in TGrid

    Area: Single;                        // Area of the cell

    Climate: TClimate;                   // Environmental variables for each cell

    GeoConn: TSngVector;                 // Degree of connectivity (1 / distance) between this cell and all other cells, based on geographic distance
    TopoConn: TSngVector;                // Degree of connectivity (1 / distance) between this cell and all other cells, based on topographic heterogeneity
    RiversConn: TSngVector;              // Degree of connectivity (1 / distance) between this cell and all other cells, based on river network
   end;

  TGrid = Record
    Cells: Array of TMapCell;
    nCells: Integer;
    EnvVec: TSngVector;                  // Store a single vector, with four entries for each grid cell (nGridCells * 4), containing Min/Max SAT, and Min/Max PPTN
    NPPVec: TSngVector;                  // Sotre a single vector, with one entry for each grid cell, containing NPP value for each grid cell
   end;
  PGrid = ^TGrid;

  // A continuous value of suitability
  Function GetLocalSuitability(LocalEnv: TEnvValues; Niche: TNicheValues): Single;

  Procedure GetLocalSuitabilities(Niche: TNicheValues; Grid: TGrid; var SuitVec: TSngVector);

  // A binary variable of tolerance, or not
  Function GetLocalTolerance(LocalEnv: TEnvValues; Niche: TNicheValues): Boolean;

  Function GetGridClimate(TimeStep: Integer; PaleoClimate: PPaleoClimate; Grid: PGrid;
                          EnvVec, NPPVec: PSngVector): Boolean;

implementation

Function GetLocalSuitability(LocalEnv: TEnvValues; Niche: TNicheValues): Single;
 var
  i, f, n: Integer;
  NicheCurve, EnvLimits, solution: TPaths;
  a, b, sigma, mi: Single;
  p, x, d, sf: Single;
  MaxFit: Single;
  StdSimBetweenCenters: Single;
  StdAreaNoOverlap: Single;
  Clipper: TClipper;
  Precision: Integer;

  {$IfDef DebugPerformance}
    c: Integer;
    start, stop, freq: Int64;
  {$EndIf}

 begin
  // Tentative result, to be updated later
  Result:= 0.0;


  // TESTING IF LOCAL ENVIRONMENT FALLS WITHIN NICHE LIMITS

  // For each environmental factor
  For f:= 0 to Length(LocalEnv)-1 do
   begin
    // Is the local environment more extreme than the limits of the tolerance/niche?
    If (LocalEnv[f].Minimum^ < Niche[f].Minimum) or
       (LocalEnv[f].Maximum^ > Niche[f].Maximum) then
     begin
      // If yes, then quit.
      // Result has been tentatively set to 0.0, so the exit here returns suitability of 0.0
      Exit;
     end;
   end;




  // The population is suitable at the cell. Lets now calculate how much
  // MEASURING HOW MUCH THE LOCAL ENVIRONMENT STRESSES THE NICHE LIMITS

  // We will use n points to draw the fitness function polygon
  n:= 10;
  Precision:= 10000;



  // Tentative result, to be updated later
  Result:= 1;

  SetLength(NicheCurve, 1);         // Niche polygon, subject to clipping by the environment
  SetLength(NicheCurve[0], n+4);    // The niche polygon will have the shape of Truncated Normal Distribution

  SetLength(EnvLimits, 1);          // Climate polygon, subjet to clipping by the niche
  SetLength(EnvLimits[0], 5);       // The environmental polygon is a box,
                                    // from min to max in the X axis, and 0 to 1 in the Y axis



{$IfDef DebugPerformance}
QueryPerformanceFrequency(freq);    // Measuring performance
QueryPerformanceCounter(start);
{$EndIf}


  // For each environmental factor to be analyzed (temperature, precipitation, etc...)
  For f:= 0 to Length(LocalEnv)-1 do
   begin

    // If there is no seasonality in the cell (max ~ min), than the cell is very suitable
    If ((LocalEnv[f].Maximum^ - LocalEnv[f].Minimum^) < 1E-2) then
     begin

      // If this is the first factor analyzed, than at least create the clipper object for the next factor
      If f = 0 then
        Clipper:= TClipper.Create;

      // If this is the last factor to be analyzed, than we can free memory as no future test will be calculate
      If f = Length(LocalEnv)-1 then
        Clipper.Free;

      // As far as this factor go, it is perfectly suitable to the population (no seasonality)
      StdAreaNoOverlap:= 1;

     end

    // The grid cell has seasonality.
    // We must now calculate the polygon clipping to find how good the cell climate is to the species
    Else
     begin

      // 1) Draw the polygon of the fitness function of the local niche

      // The curve for each factor is defined by the Truncated Normal Distribution,
      // using the niche parameters to define the minimum and maximum limits of tolerance

      // Because the TND has area equals to one, a population with narrow tolerance has
      // high suitability under that specific climatic condition.
      // Conversely, a population with broad tolerance has low suitability under a large range
      // of climatic conditions

      // Start by assuming a very small (negative) number as a fitness
      // MaxFit is a number to be searched, indicating the largest fitness of the species
      MaxFit:= -MaxSingle;

      a:= Niche[f].Minimum;                              // minimum tolerance
      b:= Niche[f].Maximum;                              // maximum tolerance
      mi:= Niche[f].Center;                              // niche center, maximum suitability
      sigma:= ((Niche[f].Maximum - Niche[f].Center)/2);  // niche breadth, with is half of the amplitude

      x:= b;            // Starting vertex of the polygon, equals to maximum tolerance
      d:= (b - a) / n;  // Interval (distance) between points

      // Draw the first vertex of the polygon
      // The polygon clipping function only uses integer values.
      // The 10000 constant is used as the precision level
      NicheCurve[0][0] := IntPoint(Trunc(x*Precision),0);

      // For each vertex to be drawn
      For i:= 0 to n do
       begin
        // Get the hight of the fitness curve, using a Truncated Normal Distribution
        // mi, sigma, a, and b are mean, standard deviation, minimum and maximum of the TND
        // x is the sampler, which is updated at every cycle of the For loop
        // The result is the probability height (fitness) of the population in the x condition
        p:= PDFNormTrunc(x, mi, sigma, a, b);

        // If p is larger than the MaxFit value found so far
        If p > MaxFit then
          MaxFit:= p;  // Update the MaxFit value

        // Insert the p found for x as a new vertex of the polygon
        // The polygon clipping function only uses integer values.
        // The 10000 constant is used as the precision level
        NicheCurve[0][i+1] := IntPoint(Trunc(x*Precision),Trunc(p*Precision));

        // Update (decrease) x, by a level of d (interval), to calculate for a new condition
        x:= x - d;
       end;

      // Insert final two vertices to close the curve polygon
      NicheCurve[0][n+2] := IntPoint(NicheCurve[0][n+1].X,0);
      NicheCurve[0][n+3]:= NicheCurve[0][0];





      // 2) Draw the boundig box of the local environment

      // The local enviroment is represented by a square/rectangle
      // The height of the rectangle is the Maximum Fitness of the species, so that the rectangle
      // has the same height as the fitness curve
      // The width of the rectangle is the range of environmental conditions in the cell
      EnvLimits[0][0] := IntPoint(Trunc(LocalEnv[f].Minimum^*Precision), 0);
      EnvLimits[0][1] := IntPoint(Trunc(LocalEnv[f].Minimum^*Precision), Trunc(MaxFit*Precision));
      EnvLimits[0][2] := IntPoint(Trunc(LocalEnv[f].Maximum^*Precision), Trunc(MaxFit*Precision));
      EnvLimits[0][3] := IntPoint(Trunc(LocalEnv[f].Maximum^*Precision), 0);
      EnvLimits[0][4]:= EnvLimits[0][0];






      // 3) Calculate the overlap between the two polygons

      // We will use a polygon clipper function to calculate the overlap, and then calculate the
      // area of the polygon formed by the overlap between the two polygons

      // If the clipper object has not been created yet, then create it
      If f = 0 then
        Clipper:= TClipper.Create;

      // Add the two polygons to the object, and clip their interction (overlap)
      Clipper.AddPaths(NicheCurve, ptSubject, true);
      Clipper.AddPaths(EnvLimits, ptClip, true);
      Clipper.Execute(ctIntersection, solution, pftNonZero, pftNonZero);

      // Do you want to know the area of the intersection?
      //WriteLn('Area Overlap 1-2: ', (Area(solution[0])/(Precision*Precision)):0:5);

      // Calculate the complement area of overlap between climate conditions and niche

      // Very high values indicate low overlap, and high suitability.
      // The high suitability (under low overlap) is a consequence of the population being subject
      // to low seasonality, therefore low environmental stress under extreme climatic conditions
      // In other words, the population is not using its most of its climate tolerance to survive

      // Very low values indicate high overlap, and low suitability
      // The low suitability (under high overlap) is a consequence of the population being subject
      // to high seasonality, therefore high environmental stress under extreme climatic conditions
      // In other words, the population is using most of its climate tolerance to survive
      StdAreaNoOverlap:= (1 - (Area(solution[0])/(Precision*Precision)));


      // If there are no further polygon overlap to be calculate, than free the object
      If f = Length(LocalEnv)-1 then
        Clipper.Free
      Else
        Clipper.Clear;
     end;



    // The standard distance between climate and niche centers is the distance between climate and niche centers,
    // divided by the half of the amplitude of the niche.
    // In other words, the overlap between niche and climatic width may be low (high suitability), but the
    // environmental conditions may be far from the niche center of the species (edge of the climatic tolerance)
    // In that case, it is necessary to account for the species having to tolerate a climatic condition that is
    // at the extreme of its niche tolerance

    // Very large values (1) indicate high suitability.
    // Very low values (0) indicate low suitability (high stress)
    StdSimBetweenCenters:= 1 - (Abs(LocalEnv[f].Center - Niche[f].Center) / ((Niche[f].Maximum - Niche[f].Minimum)/2));



    // The product of these two numbers is a standard measure of suitability at the cell,

    // Low StdSimCenters and Low AreaNoOverlap   -> Low  Suitability
    // Low StdSimCenters and High AreaNoOverlap  -> Low  Suitability
    // High StdSimCenters and Low AreaNoOverlap  -> Low  Suitability
    // High StdSimCenters and High AreaNoOverlap -> High Suitability
    sf:= StdSimBetweenCenters * StdAreaNoOverlap;



    // This product acumulates the effect of fitness across multiple environmental factors
    Result:= Result * sf;
   end;



{$IfDef DebugPerformance}
QueryPerformanceCounter(stop);
Writeln('Finished in ' + Format(' in %.1f milliseconds', [(stop - start) / freq * 1000]));
{$EndIf}

 end;

Procedure GetLocalSuitabilities(Niche: TNicheValues; Grid: TGrid; var SuitVec: TSngVector);
 var
  c: Integer;
 begin
  If Length(SuitVec) <> Grid.nCells then
    SetLength(SuitVec, Grid.nCells);

{
  TParallel.For(0, Grid.nCells-1,
    Procedure (c: Integer)
     begin
      SuitVec[c]:= GetLocalSuitability(Grid.Cells[c].Climate.EnvVars, Niche);
     end);
}

  Try
    // For each grid cell
    For c:= 0 to Grid.nCells-1 do
     begin
      // Calculate the suitability of the cell, given its climate, for the niche of a population
      SuitVec[c]:= LibTetraCells.GetLocalSuitability(Grid.Cells[c].Climate.EnvVars, Niche);
     end;
  Except
    SuitVec[c]:= 0;
   End;

  // Return a vector of suitabilities
 end;





Function GetLocalTolerance(LocalEnv: TEnvValues; Niche: TNicheValues): Boolean;
 var
  f: Integer;
 begin
  // Tentative result, to be updated later
  Result:= False;

  // TESTING IF LOCAL ENVIRONMENT FALLS WITHIN NICHE LIMITS

  // For each environmental factor
  For f:= 0 to Length(LocalEnv)-1 do
   begin
    // Is the local environment more extreme than the limits of the tolerance/niche?
    If (LocalEnv[f].Minimum^ < Niche[f].Minimum) or
       (LocalEnv[f].Maximum^ > Niche[f].Maximum) then
     begin
      // If yes, then quit.
      // Result has been tentatively set to 0.0, so the exit here returns suitability of 0.0
      Exit;
     end;
   end;

  Result:= True;
 end;





Function GetGridClimate(TimeStep: Integer; PaleoClimate: PPaleoClimate; Grid: PGrid;
                        EnvVec, NPPVec: PSngVector): Boolean;
 var
  n: Integer;
 begin
  PaleoClimate.GetClimGrid(TimeStep,
                           EnvVec,
                           NPPVec);

  n:= Length(Grid^.Cells);

  TParallel.For(0, n-1,
    Procedure (c: Integer)
     begin
      Grid^.Cells[c].Climate.EnvVars[0].Center:= ((Grid^.Cells[c].Climate.EnvVars[0].Maximum^ - Grid^.Cells[c].Climate.EnvVars[0].Minimum^) / 2) + Grid^.Cells[c].Climate.EnvVars[0].Minimum^;
      Grid^.Cells[c].Climate.EnvVars[1].Center:= ((Grid^.Cells[c].Climate.EnvVars[1].Maximum^ - Grid^.Cells[c].Climate.EnvVars[1].Minimum^) / 2) + Grid^.Cells[c].Climate.EnvVars[1].Minimum^;
     end);
 end;

end.
