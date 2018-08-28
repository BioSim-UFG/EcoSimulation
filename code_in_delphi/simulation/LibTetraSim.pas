unit LibTetraSim;

interface

{$Define DEBUG}

//{$Undef UseGPU}

uses
  System.SysUtils,
  System.SyncObjs,
  System.Threading,
  System.Math,
  Generics.Collections,

  {$IfDef UseGPU}
    CL,
    CL_Platform,
    DelphiCL,
    LibTetraGPU,
  {$EndIf}

  {$IfDef PlotMap}
    Windows,
  {$EndIf }

  LibTypes,

  LibTetraData,
  LibTetraCells;

Type
  TEventProcedure = procedure() of object;

Type
  TFounder = Record
    SeedCell: Integer;          // Starting cell of the founder
    Niche: TNicheValues;        // Starting niche of the founder
    N0: Single;                 // Starting population size of the founder
    r: Single;                  // Growth rate of the founder at the starting cell
    DispCapGeo: Single;         // Dispersal capacity of the founder over geographic distance
    DispCapRiver: Single;       // Dispersal capacity of the founder over river barriers
    DispCapTopo: Single;        // Dispersal capacity of the founder over topographic heterogeneity
   End;

  TSimParameters = Record
    SimName: String;            // Name of the simulation
    Founder: TFounder;          // Parameters defining the founder species
    StartStep: Integer;         // Begining of the simulation
    EndStep: Integer;           // End of the simulation
   End;

Type
  PSimCell = ^TSimCell;
  TSimCell = Record
    N: Single;                  // Population Size at the current time step

    K: Single;                  // Carrying capacity of the cell, calculated for every time step, and each simulation
                                // The carrying capacity is parameterized for the simulation, so must be calculated for each simulation
                                // NPP values are standard for each simulation

    g: Single;                  // Accumulated weighted population size increase

    Niche: TNicheValues;        // Niche of the population

    DispCapGeo: Single;         // Dispersal capacity of the population over geographic distance
    DispCapRiver: Single;       // Dispersal capacity of the population over river barriers
    DispCapTopo: Single;        // Dispersal capacity of the population over topographic heterogeneity

    Traits: TSngVector;         // Vector holding neutral trait values
    TraitsTmpNext: TSngVector;  // Temporary trait values to calculate updates

    CellData: PMapCell;         // Pointer to the cell holding all permanent data

    Connected: TList<PSimCell>;

    Modified: Boolean;          // Any changes in the time step?
   End;

  TSimGrid = Record
    Cells: Array of TSimCell;   // The spatial data that is specific to the simulation that is running
    PtnEnvVec: PSngVector;      // Pointer to a single vector, with four entries for each grid cell (nGridCells * 4), containing Min/Max SAT, and Min/Max PPTN
    PtnNPPVec: PSngVector;      // Pointer to a single vector, with one entry for each grid cell, containing NPP value for each grid cell
   end;

Type
  PSim = ^TSim;
  TSim = Class
    CurStep: Integer;

    DataParams: TDataParameters;
    SimParams: TSimParameters;

    TotNumCells: Integer;
    TotNumEnvVars: Integer;
    TotNumTraits: Integer;

    TotNumTimeSteps: Integer;

    RateEvolution: Single;          // Standard deviation of a normal distribution with mean zero.
                                    // The number sampled from the distribution is independently added to each trait value

    r: Single;                      // Intrinsic population growth

    SimGrid: TSimGrid;
    SimData: PSimData;

    WorkingNextStep: TEvent;
    //FinishedStep: TEvent;

    Public
      OnStartStep: TEventProcedure;
      OnFinishedStep: TEventProcedure;

      CallWhenStarting: Array of TEventProcedure;
      CallWhenFinished: Array of TEventProcedure;
      WaitWhenFinished: Array of THandle;

      Function DoNextStep: Boolean;

      Constructor Create(SimParameters: TSimParameters; SimData: PSimData);
      Destructor FreeSim;

      Procedure OnStartStepProc;
      Procedure OnFinishedStepProc;
   End;

  TSimPack = Record
    DataParams: TDataParameters;

    SimData: TSimData;

    Sims: Array of TSim;

    Function LoadSimData(LoadStream: Boolean = True; ResetAndReload: Boolean = False): Boolean;
    Function SetupNewSim(SimParams: TSimParameters): PSim;
    Function FreeAllSims: Boolean;
   End;

implementation

Uses
  LibTetraIO;



{$UnDef Parallel2}


// Creating uggly global variables for testing purposes
// These variables remain accesable over multiple calls of DoNextStep
{$IfDef UseGPU}
var
  SpNiche: TSngVector;
  SpNicheBuffer: TDCLBuffer;

  LocEnvBuffer: TDCLBuffer;

  Suitabilities: TSngVector;
  SuitabilitiesBuffer: TDCLBuffer;
{$EndIf}



Function TSim.DoNextStep: Boolean;
 var
  c, o, d, i, j: Integer;

{$IfNDef Parallel2}
 // These variables belong to the second loop
 var
  t: Integer;                        // Counters
  fg: Single;                        // Population growth
  k: Single;                         // Logistic effect of carrying capacity
  OriginCell: PSimCell;              // Link to the origin cell
  DestinationCell: PSimCell;         // Link to the destination cell
//  SaturationOrigin: Single;        // Population saturation at origin cell
//  SaturationDestination: Single;   // Population saturation at destination cell
//  SaturationGrad: Single;          // Strengh of the saturation gradient between the two cells
  Suitabilities: TSngVector;         // Suitability of the niche at the local environment
  Local_r: Single;                   // Growth rate given the local suitability (SimParameter r * Suit)
  pG, pR, pT: Single;                // Dispersal capacity parameters transformations
{$EndIf}

 begin
  // Signal the starting of a step
  OnStartStepProc;

  // Increment the step
  CurStep:= CurStep + 1;


                                                                                                                                                            {$IfDef DEBUG}
                                                                                                                                                            If CurStep = 1004 then
                                                                                                                                                              CurStep:= 1004 + 1 - 1;
                                                                                                                                                            {$EndIf}


  // Update the climate for the entire grid
  GetGridClimate(CurStep, @Self.SimData.PaleoClimate, @Self.SimData.Grid,
                 SimGrid.PtnEnvVec, SimGrid.PtnNPPVec);


  // CARRYING CAPACITY *** CARRYING CAPACITY *** CARRYING CAPACITY *** CARRYING CAPACITY *** CARRYING CAPACITY ***
  // CARRYING CAPACITY *** CARRYING CAPACITY *** CARRYING CAPACITY *** CARRYING CAPACITY *** CARRYING CAPACITY ***



{$Define Parallel1}

  // For each cell in the map
{$IfDef Parallel1}
  TParallel.For(0, TotNumCells-1,
    Procedure (c: Integer)
     var
      j: Integer;
{$Else}
    For c:= 0 to TotNumCells-1 do
{$EndIf}
     begin
      // Calculate the carrying capacity of the cell, based on NPP value at the current time step, and area of the cell
      // The carrying capacity is parameterized for the simulation, so must be calculated for each simulation, and each time step
      SimGrid.Cells[c].K:= (SimGrid.Cells[c].CellData.Climate.NPP^ * SimGrid.Cells[c].CellData.Area) / 50000;

                                                                                                                                                            {$IfDef DEBUG}
                                                                                                                                                            If c = 1506 then     // SimGrid[2459]         SimGrid[2416]
                                                                                                                                                              i:= 0;
                                                                                                                                                            {$EndIf}

      // Threshold very small carrying capacity values
      If SimGrid.Cells[c].K <= 1E-4 then
       begin
        // Too small to hold a population
        SimGrid.Cells[c].K:= 0;

        // This population will become extinct
        If SimGrid.Cells[c].N > 1E-4 then
         begin
          // Zero population size immediately
          SimGrid.Cells[c].N:= 0;

          // Zero trait values
          For j:= 0 to TotNumTraits-1 do    // Population looses all its traits
            SimGrid.Cells[c].Traits[j]:= 0;
         end;
       end;

      If (SimGrid.Cells[c].N > 1E-4) and // If there are some individuals in the cell
         (not GetLocalTolerance(SimGrid.Cells[c].CellData.Climate.EnvVars, SimGrid.Cells[c].Niche)) then // but they do not tolerate the new environment
       begin
        // Then this population will become extinct

        // Zero population size immediately
        SimGrid.Cells[c].N:= 0;

        // Zero trait values
        For j:= 0 to TotNumTraits-1 do    // Population looses all its traits
          SimGrid.Cells[c].Traits[j]:= 0;
       end;

      // Neutral evolution at each independent trait
      If SimGrid.Cells[c].N > 1E-4 then
       begin
        // Implements some mutation
        For j:= 0 to TotNumTraits-1 do
          SimGrid.Cells[c].Traits[j]:= SimGrid.Cells[c].Traits[j] + RandG(0, Self.RateEvolution)
       end;
     end{$IfDef Parallel1}){$EndIf};










                                                                                                                                                  {$IfDef DEBUG}
                                                                                                                                                  If c = 2394 then     // SimGrid[2416]
                                                                                                                                                    i:= 0;
                                                                                                                                                  {$EndIf}


  {$IfDef UseGPU}
    If CurStep = SimParams.StartStep then
     begin
      LocEnvBuffer:= CreateInputBuffer(SimData.Grid.EnvVec);

      SetLength(SpNiche, 4);
      SpNicheBuffer:= CreateInputBuffer(SpNiche);

      SetLength(LibTetraSim.Suitabilities, SimData.Grid.nCells);
      Suitabilities:= LibTetraSim.Suitabilities;
      SuitabilitiesBuffer:= CreateOutputBuffer(SimData.Grid.nCells);

      SimData.GPUKernelCalcFitness.SetArg(0, SpNicheBuffer);
      SimData.GPUKernelCalcFitness.SetArg(1, LocEnvBuffer);
      SimData.GPUKernelCalcFitness.SetArg(2, SuitabilitiesBuffer);
     end
    Else
     begin
      SimData.GPUQueue.WriteBuffer(LocEnvBuffer, (4 * SimData.Grid.nCells * SizeOf(Single)), @SimData.Grid.EnvVec[0]);
     end;
   {$EndIf}








  // IMIGRATION *** IMIGRATION *** IMIGRATION *** IMIGRATION *** IMIGRATION *** IMIGRATION *** IMIGRATION *** IMIGRATION *** IMIGRATION ***
  // IMIGRATION *** IMIGRATION *** IMIGRATION *** IMIGRATION *** IMIGRATION *** IMIGRATION *** IMIGRATION *** IMIGRATION *** IMIGRATION ***

{$IfDef Parallel2}
  // For each occupied cell in the map
  TParallel.For(0, TotNumCells-1,
    Procedure (o: Integer)
     var
      i, d, t: Integer;                  // Counter
      g: Single;                         // Maximum potential exponential grow
      k: Single;                         // Logistic effect of carrying capacity
      fg: Single;                        // Population growth
      OriginCell: PSimCell;              // Link to the origin cell
      DestinationCell: PSimCell;         // Link to each destination cell
//      SaturationOrigin: Single;        // Population saturation at Origin cell
//      SaturationDestination: Single;   // Population saturation at Destination cell
//      SaturationGrad: Single;          // Strengh of the saturation gradient between the two cells
      Suitabilities: TSngVector;         // Suitability of the niche at the local environment
      Local_r: Single;                   // Growth rate given the local suitability (SimParameter r * Suit)
      pG, pR, pT: Single;                // Dispersal capacity parameters transformations
{$Else}
    For o:= 0 to TotNumCells-1 do
{$EndIf}
     begin
      // Get the cell
      OriginCell:= @SimGrid.Cells[o];

                                                                                                                                                  {$IfDef DEBUG}
                                                                                                                                                  If o = 2394 then     // SimGrid[2416]
                                                                                                                                                    i:= 0;
                                                                                                                                                  {$EndIf}

      // Population size at the origin cell must be greater than zero
      If (OriginCell.N > 1E-3) then
       begin

        {$IfDef UseGPU}
          SpNiche[0]:= OriginCell.Niche[0].Minimum;
          SpNiche[1]:= OriginCell.Niche[0].Maximum;
          SpNiche[2]:= OriginCell.Niche[1].Minimum;
          SpNiche[3]:= OriginCell.Niche[1].Maximum;

          SimData.GPUQueue.WriteBuffer(SpNicheBuffer, (4 * SizeOf(Single)), @SpNiche[0]);

          SimData.GPUQueue.Execute(SimData.GPUKernelCalcFitness, SimData.Grid.nCells);

          SimData.GPUQueue.ReadBuffer(SuitabilitiesBuffer, (SizeOf(Single) * SimData.Grid.nCells), @LibTetraSim.Suitabilities[0]);
          Suitabilities:= LibTetraSim.Suitabilities;
        {$Else}
          // Calculate suitabilities of the population located the origin cell, given the current climate condition of all other cells
          LibTetraCells.GetLocalSuitabilities(OriginCell.Niche, SimData.Grid, Suitabilities);
        {$EndIf}




        // Population saturation at origin cell
//        SaturationOrigin:= OriginCell.N / OriginCell.K;

        // For each other cell (may include the origin cell itself)
        For d:= 0 to OriginCell.Connected.Count-1 do
         begin
          // Get the other cell TO WHERE THE GROWTH MAY GO TO
          // If c = i then growth is endogenous to the origin cell
          DestinationCell:= OriginCell.Connected[d];

                                                                                                                                                  {$IfDef DEBUG}
                                                                                                                                                  If i = 2394 then     // SimGrid[2459]
                                                                                                                                                    pG:= 0;
                                                                                                                                                  {$EndIf}

          // Immigration may only happen if there is carrying capacity in the destination cell
          If ((DestinationCell.N > 1E-3)) or
             ((DestinationCell.K > 1E-3) and
              (Suitabilities[d] > 1E-3)) then
           begin

            // Endogenous population growth
            If (OriginCell = DestinationCell) then
             begin
              // Intrinsic potential population growth and current population size at the origin cell
              fg:= (Self.r * OriginCell.N);

              // The local growth rate is the weighted by the suitability at the cell
              fg:= fg * Suitabilities[d];

              // Calculate the effect of trait, just to be consistent with migration
              For t:= 0 to TotNumTraits-1 do
                OriginCell.TraitsTmpNext[t]:= OriginCell.TraitsTmpNext[t] + (OriginCell.Traits[t] * fg);

              // Accumulates the population growth
              OriginCell.g:= OriginCell.g + fg;

              // There has been an update to the population
              OriginCell.Modified:= True;
             end

            // Exogenous population growth
            Else
             begin
{
              // Population saturation at Destination cell
              SaturationDestination:= DestinationCell.N / DestinationCell.K;

              If (o <> d) and                              // If the Destination cell is at some distance
                 (SaturationOrigin > SaturationDestination) then   // And is more saturated than the origin cell
                Continue;                                  // Then there will be no gene flow

              // Calculate the gradient of population saturation
              // If SatGrad = 0, the origin cell is empty, and individuals from the Destination cell may easily move to origin cell
              // If SatGrad = 1, the Origin cell is just as saturated as the Destination cell, and individuals do not move to origin cell
              SaturationGrad:= 1 - (SaturationOrigin / SaturationDestination);
}

              // These values transform the geographic connectivity according to the dispersal capacity parameters
              // If parameter = 0, then the transformation = 1 (no effect of connectivity)
              // If the parameter = 1, then the transformation is the actual value of connectivity (which is always less than 1)
              // If the parameter > 1, then the resistence is further increased, and the cells become more and more isolated
              pG:= OriginCell.CellData.GeoConn[d]    + ((1-OriginCell.DispCapGeo)   * (1-OriginCell.CellData.GeoConn[d])   );
              pT:= OriginCell.CellData.TopoConn[d]   + ((1-OriginCell.DispCapTopo)  * (1-OriginCell.CellData.TopoConn[d])  );
              pR:= OriginCell.CellData.RiversConn[d] + ((1-OriginCell.DispCapRiver) * (1-OriginCell.CellData.RiversConn[d]));

              // A very low connection value for any of those factors would just zero the migration
              If (pG < 1E-3) or
                 (pT < 1E-3) or
                 (pR < 1E-3) then
                Continue;

              // Calculate the population size contribution of OriginCell to DestinationCell, accounting for several factors
              fg:= ((Self.r * OriginCell.N)                           // Potential population growth and size at the Origin cell
                     //* (SaturationGrad)                             // Strength of the population saturation gradient between origin and Destination cell
                     * (OriginCell.CellData.GeoConn[d]    * pG)       // Geographic Connectivity
                     * (OriginCell.CellData.TopoConn[d]   * pT)       // Topographic Connectivity
                     * (OriginCell.CellData.RiversConn[d] * pR)       // Hidrographic Connectivity
                     * (DestinationCell.CellData.Area / 3500)         // Accounting for the size of the destination cell, assuming random walk
                   );

              // If the growth is too small... then just skip the cell before calculating suitability
              If (fg < 1E-3) then
                Continue;

              // The local growth rate is the weighted by the suitability at the cell
              fg:= fg * Suitabilities[d];

              // If the growth is too small... then just skip the cell before calculating suitability
              If (fg < 1E-3) then
                Continue;

              // Calculate the sum of trait values of all contributing cells, accounting for the magnitude of gene flow as function of population migration
              For t:= 0 to TotNumTraits-1 do
                DestinationCell.TraitsTmpNext[t]:= DestinationCell.TraitsTmpNext[t] + (OriginCell.Traits[t] * fg);

              // Accumulate the population growth over all contributing cells
              DestinationCell.g:= DestinationCell.g + fg;

              // There has been an update to the population
              DestinationCell.Modified:= True;
             end;
           end;
         end;
       end;
     end{$IfDef Parallel2}){$EndIf};









                                                                                                                                                            {$IfDef DEBUG}
                                                                                                                                                            If c = 2394 then     // SimGrid[2416]
                                                                                                                                                              i:= 0;
                                                                                                                                                            {$EndIf}





{$Define Parallel3}
  // Update the population size in each occupied cell with the changes performed in this time step
  // Reset the temporary population size in the cell
  // Flag the cell as updated
{$IfDef Parallel3}
  TParallel.For(0, TotNumCells-1,
    Procedure (c: Integer)
     var
      j, t: Integer;               // Counter
      k: Single;                // Logistic effect of carrying capacity
{$Else}
    For c:= 0 to TotNumCells-1 do
{$EndIf}
     begin

                                                                                                                                                              {$IfDef DEBUG}
                                                                                                                                                              If c = 2459 then     // SimGrid[2459]
                                                                                                                                                                i:= 0;
                                                                                                                                                              {$EndIf}

      // If there has been any modification to the population
      If SimGrid.Cells[c].Modified then
       begin

        // Velrhust logistic effect of carrying capacity
        k:= 1 - (SimGrid.Cells[c].N / SimGrid.Cells[c].K);

        // Population size in the next time step is:
        // Population size in the previous time step, plus
        // The exponential growth rate,
        // weighted by the saturation of carrying capacity
        SimGrid.Cells[c].N:= SimGrid.Cells[c].N + (SimGrid.Cells[c].g * k);

        // If the population has colapsed
        If SimGrid.Cells[c].N < 5E-1 then
         begin
          // Set it to zero
          SimGrid.Cells[c].N:= 0;

          // For each trait
          For j:= 0 to TotNumTraits-1 do    // Population looses all its traits
            SimGrid.Cells[c].Traits[j]:= 0;

          // For each niche axis
          For j:= 0 to TotNumEnvVars-1 do    // Population looses all its niche
           begin
            SimGrid.Cells[c].Niche[j].Maximum:= 0;
            SimGrid.Cells[c].Niche[j].Center:= 0;
            SimGrid.Cells[c].Niche[j].Minimum:= 0;
           end;

         end
        Else
         begin
          // Implements the in trat values due to gene flow
          // This is just a weighted average
          For t:= 0 to TotNumTraits-1 do
           begin
            SimGrid.Cells[c].Traits[t]:= (SimGrid.Cells[c].TraitsTmpNext[t] / SimGrid.Cells[c].g);
            SimGrid.Cells[c].TraitsTmpNext[t]:= 0;
           end;
         end;

        // Zero the growth
        SimGrid.Cells[c].g:= 0;

        // Reset for the new cycle
        SimGrid.Cells[c].Modified:= False;
       end;
     end{$IfDef Parallel3}){$EndIf};

  // Signal the end of a step
  OnFinishedStepProc;
 end;

Procedure TSim.OnStartStepProc;
 var
  i: Integer;
 begin
  // Set the events regarding the new step
  WorkingNextStep.ReSetEvent;
  //FinishedStep.SetEvent;

  For i:= 0 to Length(CallWhenStarting)-1 do
   begin
    if Assigned(CallWhenStarting[i]) then
      CallWhenStarting[i];
   end;
 end;

Procedure TSim.OnFinishedStepProc;
 var
  i: Integer;
 begin
  // Set the events regarding the new step
  WorkingNextStep.SetEvent;
  //FinishedStep.SetEvent;

  For i:= 0 to Length(CallWhenFinished)-1 do
   begin
    if Assigned(CallWhenFinished[i]) then
      CallWhenFinished[i];
   end;

  {$IfDef PlotMap}
    WaitForMultipleObjects(Length(WaitWhenFinished), Pointer(WaitWhenFinished), True, INFINITE);
  {$EndIf}
 end;






















Constructor TSim.Create(SimParameters: TSimParameters; SimData: PSimData);
 var
  i, j: Integer;
  pG, pR, pT: Single;       // Dispersal capacity parameters transformations
 begin
  Self.SimParams:= SimParameters;
  Self.SimData:= SimData;

  Self.DataParams:= SimData.DataParams;
  Self.TotNumCells:= SimData.TotNumCells;
  Self.TotNumEnvVars:= SimData.TotNumEnvVars;
  Self.TotNumTimeSteps:= SimData.TotNumTimeSteps;
  Self.TotNumTraits:= SimData.TotNumTraits;
  Self.RateEvolution:= SimData.RateEvolution;
  Self.r:= SimParameters.Founder.r;

  WorkingNextStep:= TEvent.Create;
  WorkingNextStep.ResetEvent;

  //FinishedStep:= TEvent.Create;
  //FinishedStep.ResetEvent;

  CurStep:= Self.SimParams.StartStep - 1;

  SimGrid.PtnEnvVec:= @SimData.Grid.EnvVec;
  SimGrid.PtnNPPVec:= @SimData.Grid.NPPVec;

  SetLength(SimGrid.Cells, TotNumCells);

  For i:= 0 to TotNumCells-1 do
   begin
    SimGrid.Cells[i].N:= 0;
    SimGrid.Cells[i].K:= 0;
    SimGrid.Cells[i].DispCapGeo:= SimParameters.Founder.DispCapGeo;
    SimGrid.Cells[i].DispCapRiver:= SimParameters.Founder.DispCapRiver;
    SimGrid.Cells[i].DispCapTopo:= SimParameters.Founder.DispCapTopo;
    SimGrid.Cells[i].CellData:= @SimData.Grid.Cells[i];

    // Create a trait vector
    SetLength(SimGrid.Cells[i].Traits, TotNumTraits);
    SetLength(SimGrid.Cells[i].TraitsTmpNext, TotNumTraits);

    // Start the trait vector
    For j:= 0 to TotNumTraits-1 do
     begin
      SimGrid.Cells[i].Traits[j]:= 0;
      SimGrid.Cells[i].TraitsTmpNext[j]:= 0;
     end;

    SetLength(SimGrid.Cells[i].Niche, TotNumEnvVars);

    // Start the niche axes
    For j:= 0 to TotNumEnvVars-1 do
     begin
      SimGrid.Cells[i].Niche[j].Maximum:= SimParameters.Founder.Niche[j].Maximum;
      SimGrid.Cells[i].Niche[j].Center:= SimParameters.Founder.Niche[j].Center;
      SimGrid.Cells[i].Niche[j].Minimum:= SimParameters.Founder.Niche[j].Minimum;
     end;

    SimGrid.Cells[i].Modified:= False;
   end;

  // Set up founder
  SimGrid.Cells[SimParameters.Founder.SeedCell].N:= SimParameters.Founder.N0;

  For j:= 0 to TotNumEnvVars-1 do
   begin
    SimGrid.Cells[SimParameters.Founder.SeedCell].Niche[j].Maximum:= SimParameters.Founder.Niche[j].Maximum;
    SimGrid.Cells[SimParameters.Founder.SeedCell].Niche[j].Center:= SimParameters.Founder.Niche[j].Center;
    SimGrid.Cells[SimParameters.Founder.SeedCell].Niche[j].Minimum:= SimParameters.Founder.Niche[j].Minimum;
   end;

   // For each cell in the grid
   For i:= 0 to Length(SimGrid.Cells)-1 do
    begin
     SimGrid.Cells[i].Connected:= TList<PSimCell>.Create;

     // For each other in the grid
     For j:= 0 to Length(SimGrid.Cells)-1 do
      begin
        pG:= SimGrid.Cells[i].CellData.GeoConn[j]    + ((1-SimParameters.Founder.DispCapGeo)   * (1-SimGrid.Cells[i].CellData.GeoConn[j]));
        pT:= SimGrid.Cells[i].CellData.TopoConn[j]   + ((1-SimParameters.Founder.DispCapTopo)  * (1-SimGrid.Cells[i].CellData.TopoConn[j]));
        pR:= SimGrid.Cells[i].CellData.RiversConn[j] + ((1-SimParameters.Founder.DispCapRiver) * (1-SimGrid.Cells[i].CellData.RiversConn[j]));

        // A very low connection value for any of those factors would just zero the migration
        If (pG > 1E-3) and
           (pT > 1E-3) and
           (pR > 1E-3) and
           ((pG * pT * pR) > 1E-3) then
         begin
          SimGrid.Cells[i].Connected.Add(@SimGrid.Cells[j]);
         end;
      end;

    end;
 end;

Destructor TSim.FreeSim;
 begin
  WorkingNextStep.Free;
  //FinishedStep.Free;
  SimGrid.Cells:= Nil;
 end;

Function TSimPack.LoadSimData(LoadStream: Boolean = True; ResetAndReload: Boolean = False): Boolean;
 begin
  SimData:= TSimData.Create(DataParams);
 end;

Function TSimPack.SetupNewSim(SimParams: TSimParameters): PSim;
 begin
  Result:= Nil;

  SetLength(Sims, Length(Sims)+1);
  Sims[Length(Sims)-1]:= TSim.Create(SimParams, @Self.SimData);

  Result:= @Sims[Length(Sims)-1];
 end;

Function TSimPack.FreeAllSims: Boolean;
 var
  i: Integer;
 begin
  For i:= 0 to Length(Sims)-1 do
    Sims[i].FreeSim;

  Sims:= Nil;
 end;
end.
