unit LibTetraData;

interface

Uses
  System.SysUtils,
  System.Classes,
  Generics.Collections,
  LibTetraGPU,
  Math,
  ZLib,

  {$IfDef UseGPU}
    CL,
    CL_Platform,
    DelphiCL,
  {$EndIf}

  //{$Define DEBUGThisUnit}

  {$IfDef DEBUGThisUnit}
    Diagnostics,
    //TimeSpan,
  {$EndIf}

  LibGIS,
  LibText,
  LibMatrix,
  LibTypes,
  LibFiles,

  LibTetraCells,
  LibPaleoData;

Type
  TEnvFactor = Record
    Name: String;
    MaxFile: String;
    MinFile: String;
   End;

  TNPPFactor = Record
    Name: String;
    OnlyFile: String;
   End;

  TDataParameters = record
    DataPath: String;               // Folder where the files below should be found
    SimPath: String;                // Folder where the simulation files should be written/read
    ResPath: String;                // Folder where the simulation results should be written/read
    CellAreaVecFile: String;        // Vector with geographic area of each cell

    TotNumCells: Integer;           // Just a shortcut. Better use Length(SimData.Grid)
    TotNumEnvVars: Integer;         // Just a shortcut. Better use Length(SimData.Grid[0].Environment)
    TotNumTimeSteps: Integer;       // Number of time steps available for environmental data
    TotNumTraits: Integer;          // Number of neutral traits (length of trait vector)

    RateEvolution: Single;          // Standard deviation of a normal distribution with mean zero.
                                    // The number sampled from the distribution is independently added to each trait value

    r: Single;                      // Intrinsic population growth

    PaleoClimFile: String;          // Stream file, containing the raw PLASIM data, in the original PLASIM grid
    PresentClimFile: String;        // Text file, containing Max/Min Temp and Precipitation, and NPP, in the simulation grid

    //EnvFiles: Array of TEnvFactor;
    //NPPFile: TNPPFactor;

    GeoDistFile: String;            // Square connectivity matrix (1 / distance) between pairs of cells based on geographic distances
    TopoDistFile: String;           // Square connectivity matrix (1 / distance) between pairs of cells based on topographic heterogeneity
    RiversDistFile: String;         // Square connectivity matrix (1 / distance) between pairs of cells based on river

    DispCapGeo: Single;         // Dispersal capacity of the founder over geographic distance
    DispCapRiver: Single;       // Dispersal capacity of the founder over river barriers
    DispCapTopo: Single;        // Dispersal capacity of the founder over topographic heterogeneity
   end;

  PSimData = ^TSimData;
  TSimData = Class
    DataParams: TDataParameters;        // Store several variables that define input parameters

    TotNumCells: Integer;           // Just a shortcut. Better use Length(SimData.Grid)
    TotNumEnvVars: Integer;         // Just a shortcut. Better use Length(SimData.Grid[0].Environment)
    TotNumTraits: Integer;          // Number of neutral traits (length of trait vector)

    TotNumTimeSteps: Integer;       // Number of time steps available for environmental data

    RateEvolution: Single;          // Standard deviation of a normal distribution with mean zero.
                                    // The number sampled from the distribution is independently added to each trait value

    r: Single;                      // Intrinsic population growth

    Grid: TGrid;                    // Store all information regarding the grid and enviromental factors

    PaleoClimate: TPaleoClimate;    // PLASIM stream data

    {$IfDef UseGPU}
      GPUQueue: TDCLCommandQueue;   // GPU CommandQueue
      GPUProgram: TDCLProgram;      // GPU Program
      GPUKernelCalcFitness: TDCLKernel;  // GPU Kernel to the function CalcFitness
    {$EndIf}

    Function LoadData(DataParameters: TDataParameters; LoadStream: Boolean = True; ResetAndReload: Boolean = False): Boolean;    // Loads simulation data, set up parameters

    Public
      Constructor Create(DataParameters: TDataParameters);
      Destructor FreeSimData;
   end;

implementation

Uses
  LibTetraIO;

Constructor TSimData.Create(DataParameters: TDataParameters);
 begin
  LoadData(DataParameters, True, True);
 end;

Destructor TSimData.FreeSimData;
 begin
  //
 end;

Function TSimData.LoadData(DataParameters: TDataParameters; LoadStream: Boolean = True; ResetAndReload: Boolean = False): Boolean;
 var
  i, j, k, v, c, t: Integer;
  DMax, DMin, D: TSngMatrix;

  Stream: TMemoryStream;
  ZipStream: TMemoryStream;
  TmpSngMat: TSngMatrix;

  AreaVec: TDblMatrix;

  {$IfDef DEBUGThisUnit}
    TmrLoading: TStopWatch;
    //TmrElapsedLoading: TTimeSpan;
  {$EndIf}

 begin
  {$IfDef DEBUGThisUnit}
    TmrLoading:= TStopwatch.Create;
    TmrLoading.Reset;
    TmrLoading.Start;

    WriteLn('Loading simulation data...');
  {$EndIf}

  DataParams:= DataParameters;

  If (LoadStream) and FileExists(DataParams.DataPath + '!SimData.sim') then
   begin
    ReadSimData(Self);
   end
  Else
   begin

    // If everything goes ok the result will be changed in the last bit of the code
    Result:= False;

  // *** CREATE A GRID *** \\\
    Try
     // How Many Cells?
     TotNumCells:= DataParameters.TotNumCells;

     // Number of environmental variables, set in the parameters
     TotNumEnvVars:= DataParameters.TotNumEnvVars;

     // Total number of time steps in the time series
     TotNumTimeSteps:= DataParams.TotNumTimeSteps;

     // Total number of traits (Length of trait vector);
     TotNumTraits:= DataParams.TotNumTraits;

     // Rate of evolution
     RateEvolution:= DataParams.RateEvolution;

     // Load the area of each cell
     AreaVec:= OpenSimple(DataParams.DataPath + DataParams.CellAreaVecFile, True);

     // Load the paleo-cliamte data
     PaleoClimate:= TPaleoClimate.Create(DataParams.DataPath + DataParams.PaleoClimFile,
                                         DataParams.DataPath + DataParams.PresentClimFile,
                                         True);

     // This vector will hold Min/Max SAT and Min/Max PPTN, for each cell
     SetLength(Grid.EnvVec, TotNumCells * 4);

     // This vector will hold NPP for each cell
     SetLength(Grid.NPPVec, TotNumCells);

     If (Grid.Cells = Nil) or (ResetAndReload) then
      begin
       Grid.nCells:= TotNumCells;

       // Create an array as long as the number of cells in the grid
       SetLength(Grid.Cells, TotNumCells);

       // For each cell in the grid
       For i:= 0 to Length(Grid.Cells)-1 do
        begin
         // Define cell id, which is the order in which it is defined the grid
         Grid.Cells[i].IdCell:= i;

         // Define cell area
         // The second column of the shapefile holds the area of the cell
         Grid.Cells[i].Area:= AreaVec[i,0];

         // Set space for environmental data, within the cell
         SetLength(Grid.Cells[i].Climate.EnvVars, TotNumEnvVars);

         // Pointers to the actual environmental data, stored in a single vector
         Grid.Cells[i].Climate.EnvVars[0].Minimum:= @Grid.EnvVec[(i*4) + 0];
         Grid.Cells[i].Climate.EnvVars[0].Maximum:= @Grid.EnvVec[(i*4) + 1];
         Grid.Cells[i].Climate.EnvVars[1].Minimum:= @Grid.EnvVec[(i*4) + 2];
         Grid.Cells[i].Climate.EnvVars[1].Maximum:= @Grid.EnvVec[(i*4) + 3];

         // Pointers to the actual NPP data, stored in a single vector
         Grid.Cells[i].Climate.NPP:= @Grid.NPPVec[i];
        end;

       // Load the square distance matrix of geographic connectance
       //LoadFromStream(TmpSngMat, DataParams.DataPath + DataParams.GeoDistFile, True);

       // Create a ZipStream
       ZipStream:= TMemoryStream.Create;
       ZipStream.Position:= 0;

       // Load the file
       ZipStream.LoadFromFile(DataParams.DataPath + DataParams.GeoDistFile);

       // Create a stream
       Stream:= TMemoryStream.Create;

       // Decompress the ZipStream
       ZDecompressStream(ZipStream, Stream);
       ZipStream.Clear;
       Stream.Position:= 0;

       // Read number of lines
       Stream.Read(k, SizeOf(Integer));

       // For each grid cell
       For i:= 0 to TotNumCells-1 do
        begin
         // Read number of columns
         Stream.Read(k, SizeOf(Integer));

         // Set up the vector of connectivity to all other cells
         SetLength(Grid.Cells[i].GeoConn, TotNumCells);
         // For each pairwaise connectivity
         For j:= 0 to TotNumCells-1 do
          begin
           // This is the reciprocal of the resistence, or connectivity
           // Large values means there is substancial connection between pairs of cells
           //Grid[i].GeoConn[j]:= TmpSngMat[i,j];
           Stream.Read(Grid.Cells[i].GeoConn[j], SizeOf(Single));
          end;
        end;

       // Clear streams
       Stream.Clear;

       // Load the square distance matrix of topographic connectance
       //LoadFromStream(TmpSngMat, DataParams.DataPath + DataParams.TopoDistFile, True);

       // Create a ZipStream
       ZipStream.Position:= 0;

       // Load the file
       ZipStream.LoadFromFile(DataParams.DataPath + DataParams.TopoDistFile);

       // Create a stream
       Stream:= TMemoryStream.Create;

       // Decompress the ZipStream
       ZDecompressStream(ZipStream, Stream);
       ZipStream.Clear;
       Stream.Position:= 0;

       // Read number of lines
       Stream.Read(k, SizeOf(Integer));

       // For each grid cell
       For i:= 0 to TotNumCells-1 do
        begin
         // Read number of columns
         Stream.Read(k, SizeOf(Integer));

         // Set up the vector of connectivity to all other cells
         SetLength(Grid.Cells[i].TopoConn, TotNumCells);
         // For each pairwaise connectivity
         For j:= 0 to TotNumCells-1 do
          begin
           // This the reciprocal of the resistence, or connectivity
           // Large values means there is substancial connection between pairs of cells
           //Grid[i].TopoConn[j]:= TmpSngMat[i,j];
           Stream.Read(Grid.Cells[i].TopoConn[j], SizeOf(Single));
          end;
        end;

       // Clear streams
       Stream.Clear;

       // Load the square distance matrix of river resistences
       //LoadFromStream(TmpSngMat, DataParams.DataPath + DataParams.RiversDistFile, True);

       // Create a ZipStream
       ZipStream.Position:= 0;

       // Load the file
       ZipStream.LoadFromFile(DataParams.DataPath + DataParams.RiversDistFile);

       // Create a stream
       Stream:= TMemoryStream.Create;

       // Decompress the ZipStream
       ZDecompressStream(ZipStream, Stream);
       FreeAndNil(ZipStream);
       Stream.Position:= 0;

       // Read number of lines
       Stream.Read(k, SizeOf(Integer));

       // For each grid cell
       For i:= 0 to TotNumCells-1 do
        begin
         // Read number of columns
         Stream.Read(k, SizeOf(Integer));

         // Set up the vector of connectivity to all other cells
         SetLength(Grid.Cells[i].RiversConn, TotNumCells);
         // For each pairwaise connectivity
         For j:= 0 to TotNumCells-1 do
          begin
           // This is the reciprocal of the resistence, or connectivity
           // Large values means there is substancial connection between pairs of cells
           //Grid[i].RiversConn[j]:= TmpSngMat[i,j];
           Stream.Read(Grid.Cells[i].RiversConn[j], SizeOf(Single));
          end;
        end;

       // Clear streams
       FreeAndNil(Stream);
      end;

    Except
      // oops, something went wrong while creating the grid
      Raise Exception.Create('Could not create grid. Missing file?');
     End;

   end;






  {$IfDef UseGPU}
    SetUpOpenCL('Libs\LibGPUTetra.cl', GPUQueue, GPUProgram);
    GPUKernelCalcFitness:= GetKernel(GPUProgram, 'CalcFitness');
  {$EndIf}





  // Write all simulation data to the hard drive as a stream file
  //WriteSimData(Self);

  // Hooray! Simulation set up! Ready to start the time cycles!
  Result:= True;

  {$IfDef DEBUGThisUnit}
    TmrLoading.Stop;
    // TmrElapsedLoading:= TmrLoading.Elapsed

    WriteLn('Simulation data loaded in ' + FloatToStr(TmrLoading.ElapsedMilliseconds/1000) + ' seconds');
  {$EndIf}

 end;

end.
