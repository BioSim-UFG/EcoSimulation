unit LibTypes;
//{$D-}
{$mode delphi}{$H+}


interface

{$Define MtxVec}
//{$IfDef MtxVec}

uses
  {$IFDEF MSWINDOWS}
    {$IFDEF VER230UP}      // Delphi XE2
      WinApi.Windows,
    {$ELSE}
      Windows,
    {$ENDIF}
  {$ELSE}
    {$IFDEF LINUX}
    {$ENDIF}

    {$IFDEF IOS}
    {$ENDIF}

    {$IFDEF MACOS}
    {$ENDIF}

    {$IFDEF ANDROID}
    {$ENDIF}
  {$ENDIF}


  SysUtils;

(*
 const
  NULL            =  #0;
  BACKSPACE       =  #8;
  TAB             =  #9;
  LF              = #10;
  CR              = #13;
  EOF_            = #26;    { 30.07.96 sb }
  ESC             = #27;
  BLANK           = #32;
  SPACE           = BLANK;
  CRLF            : PChar = CR+LF;

  { digits as chars }
  ZERO   = '0';  ONE  = '1';  TWO    = '2';  THREE  = '3';  FOUR  = '4';
  FIVE   = '5';  SIX  = '6';  SEVEN  = '7';  EIGHT  = '8';  NINE  = '9';

  { special codes }
  SLASH           = '\';     { used in filenames }
  HEX_PREFIX      = '$';     { prefix for hexnumbers }


  { common computer sizes }
  KBYTE           = Sizeof(Byte) shl 10;
  MBYTE           = KBYTE        shl 10;
  GBYTE           = MBYTE        shl 10;

  { Low floating point value }
  FLTZERO         : TFloat = 0.00000001;

  MINSHORTINT    = -128;               { 1.8.96 sb }
  MAXSHORTINT    =  127;
  MINBYTE        =  0;
  MAXBYTE        =  255;
  MINWORD        =  0;
  MAXWORD        =  65535;
*)

Var
  Decs: ShortInt = 3;
  FontSize: ShortInt = 9;

Type // Tipos Básicos
  {$IfNDef FMX}
  TFloat = Double;
  {$Else}
  TFloat = Single;
  {$EndIf}

  TExtVector = Array of Extended;
  TDblVector = Array of TFloat;
  PDblVector = ^TDblVector;
  TIntVector = Array of Integer;
  PIntVector = ^TIntVector;
  TWrdVector = Array of Word;
  TCrdVector = Array of Cardinal;
  TStrVector = Array of String;
  {$IFDEF MSWINDOWS}
  TShtStrVector = Array of ShortString;
  {$ENDIF}
  TPCharVector = Array of PChar;
  TBolVector = Array of Boolean;
  TBytVector = Array of Byte;
  TSngVector = Array of Single;
  PSngVector = ^TSngVector;

  {$IFDEF MSWINDOWS}
  TAnsStrVector = Array of AnsiString;
  TAnsStrMatrix = Array of TAnsStrVector;
  {$ENDIF}

  TStrMatrix = Array of TStrVector;
  PStrMatrix = ^TStrMatrix;
  TPCharMatrix = Array of TPCharVector;
  TDblMatrix = Array of TDblVector;
  PDblMatrix = ^TDblMatrix;
  TExtMatrix = Array of TExtVector;
  TIntMatrix = Array of TIntVector;
  TBolMatrix = Array of TBolVector;
  TBytMatrix = Array of TBytVector;
  TSngMatrix = Array of TSngVector;

  TIntTriMatrix = Array of TIntMatrix;
  TDblTriMatrix = Array of TDblMatrix;
  TStrTriMatrix = Array of TStrMatrix;

//  TExtVector = Array of TFloat;
//  TExtMatrix = Array of TExtVector;
//  TExtTriMatrix = Array of TExtMatrix;


  TFindKDist = Record
    MinDistVal: Double;
    MinList: TIntVector;
    MaxDistVal: Double;
    MaxList: TIntVector;
  End;


Type     // Phylogeny Types
  TBranchType = (btNil, btNode, btTip, btNodes, btTips, btAll);
  TBranchTypeVector = Array of TBranchType;

  TBranchPointer = Pointer;
  TBranchPointerVector = Array of Pointer;



Type
  TEvolModel = (TEMNone,        // No transformation

                TEMDefault,     // Whatever is default in TPAMData.PhyTrans

                TEMBrownMot,    // Brownian motion model
                                // Constant rates random walk: raw distances

                TEMLambda,      // Pagel's lambda
                                // Estimate phylogenetic signal by multiplying all internal branches of the
                                // tree by lambda, leaving tip branches as their original length (root to tip
                                // distances are unchanged

                TEMKappa,       // Pagel's kappa
                                // Raise all branch lengths to the power of kappa. As kappa approaches zero,
                                // trait change becomes focused at branching events. For complete phylogenies,
                                // if kappa approaches zero this infers speciational trait change

                TEMDelta,       // Pagel's delta
                                // Raise all node depthts (heights) to the power of delta.
                                // If delta < 1, trait evolution is concentrated early in the three, whereas
                                // if delta > 1 trait evolution is concentrated towards the tips. Values of
                                // delta above one can be difficult to fit reliably

                TEMOU,          // Ornstein-Uhlenbeck
                                // Random walk with a central tendency proportional to alpha. High values
                                // of alpha can be interpreted as evidence of evolutionary constraits,
                                // stabilising selection or weak phylogenetic signal

                TEMPsi);        // Psi - !!! NOT IMPLEMENTED !!!
                                // fits a model to assess to the relative contributions of speciation
                                // and gradual evolution to a trait's evolutionary rate (Ingram 2010).
                                // Psi = 0 (gradual) or Psi = 1 (speciational)

  TTransformTree = Record
    EvolModel: TEvolModel;
    Parameter: Double;
   End;

Type
  TColDataType = (TCDTUnknown, TCDTNumeric, TCDTText);
  TColDataTypesVec = Array of TColDataType;

  TDataSet = record
    ColDataType: TColDataTypesVec;
    FullVarNames: TStrVector;

    NumValues: TDblMatrix;
    NumVarNames: TStrVector;

    StrValues: TStrMatrix;
    StrVarNames: TStrVector;

    Function GetStrValue(StrVar: String; Index: Integer): String;
    Function GetNumValue(NumVar: String; Index: Integer): TFloat;
    Function HasStrVar(StrVar: String): TIntVector;

    Procedure AddNumVec(Vector: TDblVector; VecName: String = ''; p: Integer = -1);
    Procedure AddNumMat(Matrix: TDblMatrix; MatNames: TStrVector = Nil);

    Procedure ReSet;
{
    case HasText: Boolean of
      True: (
            StrValues: TFloat; //TStrMatrix;
            StrVarNames: TFloat; //TStrVector;
            );
}
   end;

// Originally from UntFuncsGeometry
(**************[  Vertex type   ]***************)
type TPoint2DPtr    = ^TPoint2D;
     TPoint2D       = record X, Y: TFloat; end;
     TPoint2DVec    = array of TPoint2D;
     TPoint2DArray  = array of TPoint2D;
     PPoint2DArray  = ^TPoint2DArray;

type TPolygon2D     = array of TPoint2D;
type TPolygon2DPtr  = ^TPolygon2D;

type TPolyLine2D     = array of TPoint2D;
type TPolyLine2DPtr  = ^TPolygon2D;



Type
	TBoundBox = Record
		Min : TPoint2D;
		Max : TPoint2D;
	End;




{ Cada TShp2DPart é de fato um polígono, porém pode ser uma área real ou um buraco em outro polígono }
Type
  TShpPolygon2DPart = record                 // Alguns polígonos possuem vários sub-polígonos, apesar de terem apenas uma referencia a um DBF record
    Vertices: TPolygon2D;             // Esta é a lista de vértices de um polígono
    Area, Perimeter: Double;
    Centroid: TPoint2D;
    BoundingBox: TBoundBox;
    Hole: Boolean;                    // Isto permite que haja "buracos" no polígono, caso "Hole" seja True
   end;

{ Cada TShpPolygon pode conter vários TShp2DPart, porém se refere a apenas um record no DBF }
Type
  TShpPolygon2D = record                // Para cada um dos TShpPolygon haverá um record no DBF
    Part: Array of TShpPolygon2DPart;   // Esta é uma lista de polígonos, que se refere a um record no DBF
    Area, Perimeter: Double;
    Centroid: TPoint2D;
    BoundingBox: TBoundBox;
    DBFRecord: Cardinal;
   end;
  TShpPolygon2DArray = Array of TShpPolygon2D;

{ Este é um Shapefile tipo Polygon }
Type
  TShp2DPolygonType = record          // No futuro, para o ShapeFile todo, com o centroid, área, perímetro e box
    Polygon: TShpPolygon2DArray;
    Area, Perimeter: Double;
    Centroid: TPoint2D;
    BoundingBox: TBoundBox;
   end;
  PShp2DPolygonType = ^TShp2DPolygonType;






{ Cada TShp2DPart é de fato uma linha}
Type
  TShpPolyLine2DPart = record                 // Algumas linhas possuem várias sub-linhas, apesar de terem apenas uma referencia a um DBF record
    Vertices: TPolyLine2D;                     // Esta é a lista de vértices da linha
    Length: Double;
    Centroid: TPoint2D;
    BoundingBox: TBoundBox;
   end;

Type
  TShpPolyLine2D = record                // Para cada um dos TShpPolyLines haverá um record no DBF
    Part: Array of TShpPolyLine2DPart;   // Esta é uma lista de linhas, que se refere a um record no DBF
    Length: Double;
    Centroid: TPoint2D;
    BoundingBox: TBoundBox;
    DBFRecord: Cardinal;
   end;
  TShpPolyLine2DArray = Array of TShpPolyLine2D;

Type
  TShp2DPolyLineType = record
    PolyLine: TShpPolyLine2DArray;
    Length: Double;
    Centroid: TPoint2D;
    BoundingBox: TBoundBox;
   end;
  PShp2DPolyLineType = ^TShp2DPolyLineType;

Type
  TClass = Record
    LwBound, Center, UpBound: TFloat;
   end;
  TClasses = Array of TClass;





Type // Tipos de dados estatísticas descritivas
  TBasStats1 = Record
    n: Cardinal;
    Min: TFloat;
    Mid: TFloat;
    Max: TFloat;
    Range: TFloat;
    Sum: TFloat;
    Mean: TFloat;
    StdDev: TFloat;
   end;
  TBasStats2 = Record
    Min: TFloat;
    Max: TFloat;
    LwQuart: TFloat;
    Mean: TFloat;
    Median: TFloat;
    UpQuart: TFloat;
    StdDev: TFloat;
    Sum: TFloat;
    Sum2: TFloat;
   end;
  TBasStats3 = Record
    n: Integer;
    Min: TFloat;
    Max: TFloat;
    LwQuart: TFloat;
    Mean: TFloat;
    Median: TFloat;
    UpQuart: TFloat;
    StdDev: TFloat;
    Sum: TFloat;
    Sum2: TFloat;
    Skew: TFloat;
    SES: TFloat;
    Kurt: TFloat;
    SEK: Extended;
   end;

Type
  TEigenResults = Record
    EValues: TDblVector;
    EVectors: TDblMatrix;
    nRot: Word;
   end;

Type
  TSVDResults = Record
    S: TDblVector;
    V: TDblMatrix;
    U: TDblMatrix;
   End;

Type

  TRegResults = Record
    b, bStd, t, SEb: TDblVector;
    Est, Res: TDblVector;
    r2, r2Adj, F, Probr2, AIC: TFloat;
    ESS, KL, JS, Hel, Euclid, L1: TFloat;
   end;

  TParRegOp = (prPlus, prDot, prColon, prSub, prProc);  // adição (agrupamento), controle do efeito ou interação
  TParRegOpVec = Array of TParRegOp;

  TParRegCtrl = Record
    Elements1: TIntVector;
    Operation: TParRegOp;
    Elements2: TIntVector;
   end;
  TParRegCtrlVec = Array of TParRegCtrl;
  TParRegResult = Record
//    Reg: TRegResults;
    s: String;
    r2: TFloat;
    o, c: TIntVector;
    Op: TParRegOpVec;
    Ctrl: TParRegCtrlVec;
   end;

  TParRegResults = Array of TParRegResult;

  TModelRes = record
    CN, R2, R2Adj, IMoran, ISobIMax, ResStdDev: TFloat;
    AICc, dAICc, LLAICc, AICcw, EvidRatio: TFloat;
    RegResults: TRegResults;
    m: Integer;
    p: Array of Cardinal;
   end;

  TMultModelRes = record
   Models: Array of TModelRes;
   MultModel: TRegResults;
   VarRanks: Array of TFloat;
   ModelRank: Array of ^TModelRes;
  end;

Type
  TLogRegOddRatios = Record
    OddRat, Low, High: TFloat;
   end;

  TBinaryEval = Record
    a, b, c, d: Integer;
    Accuracy, Sensitivity, Specificity, Kappa, TSS: TFloat;
   end;

  TLogRegResults = Record
    Prob, Est, Res: TDblVector;
    b, bStd, t, SEb: TDblVector;
    AIC, LogLikelihoodNull, LogLikelihoodFit: TFloat;
    ChiSq, df, P, r2: TFloat;
    OddRat: Array of TLogRegOddRatios;
    ModelEval: TBinaryEval;
    Accuracy, Sensivity, Specificity, Kappa, TSS: TFloat;

    ItHist: TDblVector;
    Table : array[1..3,1..3] of integer;
   end;

Type
  TGLSResults = Record
    b, bStd, t, SEb: TDblVector;
    Est, Res, Err: TDblVector;
    r2, r2Adj, F, Pr2, AICc, r2Full, r2AdjFull, FFull, Pr2Full, AICcFull: TFloat;
    OLS: TRegResults;
  end;

Type
  TKResults = Record
    MSE, MSE0, MSE0MSE: Double;
    LogLikelihood, LogLikY, AIC, AICc: TFloat;
    K: Double;
   End;

Type
  TPGLSResults = Record
    b, bStd, t, SEb: TDblVector;
    Est, Res, Err: TDblVector;
    r2, r2Adj, F, Pr2, Sigma2: TFloat;
    Cov: TDblMatrix;

    LogLikelihood, LogLikY, AIC, AICc: TFloat;
    K: TKResults;
  end;

Type
  TLambdaResults = Record
    Lambda: TFloat;
    LogLikeY: TFloat;
    PGLS: TPGLSResults;
  end;

Type
  TAlphaResults = Record
    Alpha: TFloat;
    LogLikeY: TFloat;
    PGLS: TPGLSResults;
  end;

Type
  TEvolModelResults = Record
    Parameter: TFloat;
    LogLikeY: TFloat;
    PGLS: TPGLSResults;
  end;

Type
  TPGLSSearchResults = Record
    PhyTransform: TTransformTree;
    LogLikeY: TFloat;
    PGLS: TPGLSResults;
  end;

Type
  TpGLSResultsShort = Record
    b: TDblVector;
    LogLikelihood, LogLikeY, AIC, AICc: TFloat;
    EvolModel: TEvolModelResults;
    K: TKResults;
   End;
  TpGLSResultsShortVec = Array of TpGLSResultsShort;

Type
  TMRho3Results = Record
    Est, Res: TDblVector;
    Rho, StdRho, Z, F: TFloat;
    r2, r2Adj, AIC, U2: TFloat;
    VarY: TFloat;
   end;

Type
  TCorrelationResults = Record
    Var1, Var2, Cov12: TFloat;
    R, F, P: TFloat;
   end;

Type
  TVIFResults = Record
    r2: TDblVector;
    VIF: TDblVector;
    MeanVIF: TFloat;
   end;

Type
  TMulticolinearityResults = Record
    ConditionNumber: TFloat;
    FstEigenValOverm: TFloat;
    MeanCorrelMat: TFloat;
    VIF: TVIFResults;
   end;

Type // Tipos de dados do GWR
  TGWRSearchMethods = (AICc, GCV, r2);
  TGWRSearchParams = Record
   hMin: TFloat;
   hMax: TFloat;
  end;
  TGWRSearch = Record
   Params: TGWRSearchParams;
   Method: TGWRSearchMethods;
   StopRule: TFloat;
  end;
  TGWRSptFunc = (GWRMovingWin, GWRGaussian, GWRBiSquared);
  TGWROpt = Record
   SptFunc: TGWRSptFunc;
   AdaptKernel: Boolean;
   h: TFloat;        // Caso use GWRSearch, estes h's são resultados: h=dist; h2=AdaptBandWidth
  end;
  TGWRAnova = Record
    RSSOLS, RSSGWR, RSSImp: TFloat;
    DFOLS, DFGWR, DFImp: TFloat;
    MSGWR, MSImp: TFloat;
    F: TFloat;
   end;
  TGWROptResult = Record
    h, h2, AICc, GCV, r2: TFloat;
   end;
  TGWROptTriplet = Array of TGWROptResult;
  TGWRq = Record
   b, bStd, SEb, t, P: TDblVector;
   Est, Res: TFloat;
   Residuals: TDblVector;
   Estimated: TDblVector;
   Hat: TFloat;
   r2, Adjr2, F, Pr2, AICc: TFloat;
   h, h2: TFloat;
  end;
  TGWRResults = Record
    q: Array of TGWRq; // Resultados GLS por quadrícula
    Residuals, StudResiduals, StadResiduals, CookD: TDblVector;
    Estimated: TDblVector;
    Hat: TDblVector;           // Sigma=Variancia dos resíduos
    r2, Adjr2, F, Pr2, AICc, GCV, Sigma, m, RSS, h, h2: TFloat;
    OLSr2, OLSAdjr2, OLSAICc, OLSRSS, OLSF, OLSPr2, OLSSigma: TFloat;
    ANOVA: TGWRAnova;
  end;

Type
  TMoranClass = Record
    nPairs: Cardinal;
    IMoran, IMaxMoran, IPerMaxMoran, IStdErr, IP, IExpect: TFloat;
    Wy: TDblVector;
    GeryC: TFloat;
    GetisOrdG: TFloat;
    SemiVar: TFloat;

    MinCI, MaxCI: TFloat;

    DistClass: TClass;
    AngleClass: TClass;
   end;
  PMoranResults = Array of ^TMoranClass;
  TMoranResults = Array of TMoranClass;

Type
  TMoranClassShort = Record
    nPairs: Cardinal;
    IMoran, IMaxMoran, IPerMaxMoran: TFloat;
    DistClass: TClass;
   end;
  PMoranResultsShort = Array of ^TMoranClassShort;
  TMoranResultsShort = Array of TMoranClassShort;

Type
  TJoinCountClass = Record
    nPairs: Cardinal;
    sBB, sWW, sBWWB: Cardinal;

    nb, nw, nt: Cardinal;
    sL, SLL_1, L, K: UInt64;
    pb, pw, pbwwb: TFloat;
    eBB, eWW, eBWWB: TFloat;
    stddevBB, stddevWW, stddevBWWB: TFloat;
    zBB, zWW, zBWWB: TFloat;
    pzBB, pzWW, pzBWWB: TFloat;

    DistClass: TClass;
    AngleClass: TClass;
   end;
  PJoinCountResults = Array of ^TJoinCountClass;
  TJoinCountResults = Array of TJoinCountClass;

Type
  TMantelClass = Record
    nX, nT: Cardinal;     // nX pares dentro das classes, nT pares total
    zX, zT: Extended;     // zX dentro das classes, zT total!
    zX_n, zT_n: Extended; // zX_n dentro da classe, zT_n total!
    rX, rT: TFloat;       // r dentro da classe, e r total!
    POneTailUp, POneTailLw, PTwoTail: TFloat;
    DistClass: TClass;
   end;

  PMantelResults = Array of ^TMantelClass;
  TMantelResults = Array of TMantelClass;

Type
  TMantelPartialResults = Record
    MantelAB: TMantelResults;
    MantelAC: TMantelResults;
    MantelBC: TMantelResults;
    rAB_C: Double;
    POneTailUp: Double;
    POneTailLw: Double;
    PTwoTail: Double;
   End;

Type
  TLISAPerCell = Record
    nPairs: Integer;
    IMoran, GeryC, GetisOrdG: TFloat;
    IP: TFloat;
   end;
  TLISAPerCellShort = Record
    nPairs: Integer;
    IMoran: TFloat;
   end;
  TLISAPerClass = Record
    Classes: Array of TLISAPerCell;
   end;
  TLISAPerClassShort = Record
    Classes: Array of TLISAPerCellShort;
   end;
  TLISAResults = Record
    Cells: Array of TLISAPerClass;
    DistClasses: TClasses;
   end;
  TLISAResultsShort = Record
    Cells: Array of TLISAPerClassShort;
   end;
  PLISAResults = Record
    Cells: Array of ^TLISAPerClass;
    DistClasses: ^TClasses;
   end;

Type
  TGetisFilter = Record
    nPairs: TIntVector;
    G, XStar, L: TDblVector;
    LagDist: TDblVector;
   end;

Type
  TSpatialFiltersOptions = Record
    TruncDist: TFloat;
    MatrixW: TDblMatrix;
   end;
  TSpatialFiltersResults = Record
    EValues: TDblVector;
    EVectors: TDblMatrix;
    MoranVectors: TDblVector;
   end;

Type
   TNestedANOVAGroups = Record
     SumOfSquares: Double;
     DegFred: Integer;
     MeanSquares: Double;
     F: Double;
     P: Double;
     VarCompRaw: Double;
     VarCompProp: Double;
     CoefVar: Double;
    End;

   TNestedANOVASubGroup = Record
     SubGroupId: Integer;

     n: Integer;
     Sum_n2: Integer;

     Sum: Double;
     Sum2: Double;
     Mean: Double;
     Sum2Dev: Double;
     nXSum2Dev: Double;
    End;

   TNestedANOVAGroup = Record
     GroupId: Integer;

     n: Integer;
     Sum_n2: Integer;
     Sum_n2_Div_n: Double;

     Sum: Double;
     Sum2: Double;
     Mean: Double;
     Sum2Dev: Double;
     nXSum2Dev: Double;

     SubGroupStats: Array of TNestedANOVASubGroup;
    End;

   TNestedANOVARes = Record
     AmongGroups: TNestedAnovaGroups;
     SubGroupsWithinGroups: TNestedAnovaGroups;
     WithinSubGroups: TNestedAnovaGroups;
     TotalGroups: TNestedAnovaGroups;

     nbo: Double;
     no: Double;
     noprime: Double;
     w1: Double;
     w2: Double;

     nGroups: Integer;
     nSubGroups: Integer;

     Grandn: Integer;
     GrandSum_n2: Integer;
     GrandSum_n2b: Integer;
     GrandSum_n2_Div_n: Double;

     GrandSum: Double;
     GrandSum2: Double;
     GrandMean: Double;
     GrandSum2Dev: Double;

     GroupStats: Array of TNestedANOVAGroup;
    End;

{$IfNDef BE}
{$IfNDef FPC}
Type
  TSpecies = Record
    Name: String;
    Selected: Boolean;
    PUT: Boolean;
  End;
  TSpeciesVec = Array of TSpecies;
{$EndIf}
{$EndIf}

Type
  TSine = Record
    x: TFloat;        // position in the space (coordinate) { Used only in n dimensional waves, not 1D or 2D }
    nWaves: TFloat;   // Number of waves
   end;
  TSineVector = Array of TSine;

  TSineDomain = Record
    x: TFloat;                    // Relative position in the space (spatial coordinate)
    sMin, sMid, sMax: Double;     // Domain limits
   End;
  TSineDomains = Array of TSineDomain;

  TSineWave2D = Record
    XAxis: TSineDomain;
    YAxis: TSineDomain;
    TAxis: TSineDomain;
    nWaves: TFloat;           // Number of waves
    Weight: TFloat;           // Weight given to each wave/complex
    Lag: TFloat;              // Após uma curva completa (x = 1), acrescenta um espaço entre a próxima curva
   end;
  TSineWaveVector = Array of TSineWave2D;
  TSineWaveMatrix = Array of TSineWaveVector;

  TSineWaveMapPar = Record
    X: Double;
    XMin, XMid, XMax, XInc: Double;    // Long Axis: Used to grid the map
    XnStep: Integer;

    Y: Double;
    YMin, YMid, YMax, YInc: Double;    // Lat Axis: Used to grid the map
    YnStep: Integer;

    T: Double;
    TMin, TMax, TInc: Double;          // Time Axis: Used to grid the map
    TnStep: Integer;

    SineWaves: TSineWaveVector;
   End;

{$IfDef MtxVec}
Type
  TProd2MatrixRec = Record
    M1, M2, Res: TDblMatrix;
   end;
Type
  TEigenRec = Record
    A: TDblMatrix;
    D: TDblVector;
    V: TDblMatrix;
   end;
Type
  TSVDRec = Record
    M: TDblMatrix;
    S: TDblVector;
    V: TDblMatrix;
    U: TDblMatrix;
   end;
Type
  TDeterminantRec = Record
    M: TDblMatrix;
    Det: TDblVector;
   end;
Type
  TGaussJordanRec = Record
    M: TDblMatrix;
   end;
Type
  TCholdcRec = Record
    A: TDblMatrix;
  End;
  TLUDecompRec = Record
    A: TDblMatrix;
  End;
  TLowTrigRec = Record
    A: TDblMatrix;
  End;
  TInvRec = Record
    A: TDblMatrix;
  End;
  TCauchyRec = Record
    Location: Double;
    Shape: Double;
    nSamp: Integer;
    Res: TDblVector;
  end;
{$EndIf}

Type
 TCombinatorial = Record
   b, l, m: Integer;
   Orig, Pntr, Ord: TIntVector;

   Procedure ReSet(t: Integer);    overload;
   Procedure ReSet(p: TIntVector); overload;
   Function Next: Boolean;
  end;

  Function IntToDbl(Vector: TIntVector): TDblVector;
  Function DblToInt(Vector: TDblVector): TIntVector;

implementation

Function IntToDbl(Vector: TIntVector): TDblVector;
 var
  i: Integer;
 begin
  SetLength(Result, Length(Vector));
  For i:= 0 to Length(Vector)-1 do
    Result[i]:= Vector[i];
 end;
Function DblToInt(Vector: TDblVector): TIntVector;
 var
  i: Integer;
 begin
  SetLength(Result, Length(Vector));
  For i:= 0 to Length(Vector)-1 do
    Result[i]:= Trunc(Vector[i]);
 end;

Procedure TCombinatorial.ReSet(t: Integer);
 var
  i: Integer;
  p: TIntVector;
 begin
  SetLength(p, t);
  for i:= 0 to t-1 do
    p[i]:= i;
  ReSet(p);
 end;

Procedure TCombinatorial.ReSet(p: TIntVector);
 begin
  Orig:= p;
  SetLength(Orig, Length(Orig));

  m:= Length(Orig);
  b:= 1;
  l:= 0;

  SetLength(Pntr, 1);
  Pntr[0]:= -1;
 end;

Function TCombinatorial.Next: Boolean;
 Function NxtOrd: Boolean;
  var
   i: Integer;
  begin
   if b > m then
     Result:= False
   Else
    begin
     SetLength(Pntr, b);
     for i:= 0 to b-1 do
       Pntr[i]:= i;
     Result:= True;
    end;
  end;
 var
  i, p, v: Integer;
 label
  1;
 begin
  Result:= False;

  p:= b-1;
  While True do
   begin
    l:= l + 1;

    if Pntr[0] = (m-(b-1))-1 then
      Break;

    if Pntr[p] < m-1 then
      Pntr[p]:= Pntr[p] + 1
    Else
     begin
      v:= 0;
      while Pntr[p] = (m-v)-1 do
       begin
        p:= p-1;
        v:= v + 1;
       end;
      Pntr[p]:= Pntr[p] + 1;
      for i:= p+1 to Length(Pntr)-1 do
        Pntr[i]:= Pntr[p]+(i-p);
      p:= b-1;
     end;

    Result:= True;
    GoTo 1;
   end;

  b:= b + 1;
  if NxtOrd then
    Result:= True
  Else
    Result:= False;

1:
  if Result then
   begin
    SetLength(Ord, Length(Pntr));
    For i:= 0 to Length(Pntr)-1 do
      Ord[i]:= Orig[Pntr[i]];
   end
  Else
   begin
    Pntr:= Nil;
    Ord:= Nil;
   end;
 end;


Function TDataSet.GetStrValue(StrVar: String; Index: Integer): String;
 var
  i: Integer;
 begin
  Result:= '';
  For i:= 0 to Length(StrVarNames)-1 do
   begin
    If StrVarNames[i] = StrVar then
     begin
      Result:= StrValues[Index,i];
      Break;
     end;
   end;
 end;

Function TDataSet.GetNumValue(NumVar: String; Index: Integer): TFloat;
 var
  i: Integer;
 begin
  Result:= 0;
  For i:= 0 to Length(NumVarNames)-1 do
   begin
    If NumVarNames[i] = NumVar then
     begin
      Result:= NumValues[Index,i];
      Break;
     end;
   end;
 end;

Function TDataSet.HasStrVar(StrVar: String): TIntVector;
 var
  i: Integer;
 begin
  Result:= Nil;
  For i:= 0 to Length(StrVarNames)-1 do
   begin
    If StrVarNames[i] = StrVar then
     begin
      SetLength(Result, Length(Result)+1);
      Result[Length(Result)-1]:= i;
     end;
   end;
 end;

Procedure TDataSet.AddNumVec(Vector: TDblVector; VecName: String = ''; p: Integer = -1);
 var
  i, j, k: Integer;
  New: Boolean;
 begin
  New:= False;

  If NumValues = Nil then
   begin
    SetLength(NumValues, Length(Vector), 1);

    SetLength(NumVarNames, 1);
    NumVarNames[0]:= VecName;

    New:= True;
   end;

  If (p > Length(NumValues[0])-1) or (P < 0) then // add a new column to the right of the matrix
   begin
    If not New then
     begin
      SetLength(NumValues, Length(NumValues), Length(NumValues[0])+1);

      SetLength(NumVarNames, Length(NumVarNames)+1);
      NumVarNames[Length(NumVarNames)-1]:= VecName;
     end;

    k:= Length(NumValues[0])-1;      // new column
    For i:= 0 to Length(Vector)-1 do
      NumValues[i,k]:= Vector[i];
   end
  Else                             // create a new column at c1, and move all data to the right
   begin
    SetLength(NumValues, Length(NumValues), Length(NumValues[0])+1);
    SetLength(NumVarNames, Length(NumVarNames)+1);

    For i:= 0 to Length(NumValues)-1 do
     begin
      For j:= Length(NumValues[0])-1 downto p+1 do
        NumValues[i,j]:= NumValues[i,j-1];

      NumValues[i,p]:= Vector[i];
     end;

    For j:= Length(NumVarNames)-1 downto p+1 do
      NumVarNames[j]:= NumVarNames[j-1];

    NumVarNames[p]:= VecName;
   end;
 end;

Procedure TDataSet.AddNumMat(Matrix: TDblMatrix; MatNames: TStrVector = Nil);
 var
  i, j, k: Integer;
 begin
  If Matrix = Nil then
    Exit;

  If NumValues = Nil then
   begin
    NumValues:= Matrix;
    SetLength(NumValues, Length(NumValues), Length(NumValues[0]));

    NumVarNames:= MatNames;
    SetLength(NumVarNames, Length(NumVarNames));

    Exit;
   end;

  If (Length(NumValues) <> Length(Matrix)) then
    Raise Exception.Create('Matrices can not be joined!')
  Else
   begin
    k:= Length(NumValues[0]);
    SetLength(NumValues, Length(NumValues), Length(NumValues[0]) + Length(Matrix[0]));

    For i:= 0 to Length(NumValues)-1 do
     begin
      For j:= k to Length(Matrix[0])-1 do
        NumValues[i,j]:= Matrix[i,j-k];
     end;

    k:= Length(NumVarNames);
    SetLength(NumValues, Length(NumVarNames) + Length(MatNames));
    For j:= k to Length(MatNames)-1 do
      NumVarNames[j]:= MatNames[j-k];
   end;
 end;

Procedure TDataSet.ReSet;
 begin
  Self.ColDataType:= Nil;
  Self.FullVarNames:= Nil;
  Self.NumValues:= Nil;
  Self.NumVarNames:= Nil;
  Self.StrValues:= Nil;
  Self.StrVarNames:= Nil;
 end;

end.
