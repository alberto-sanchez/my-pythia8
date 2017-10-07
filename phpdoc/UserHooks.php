<html>
<head>
<title>User Hooks</title>
</head>
<body>

<script language=javascript type=text/javascript>
function stopRKey(evt) {
var evt = (evt) ? evt : ((event) ? event : null);
var node = (evt.target) ? evt.target :((evt.srcElement) ? evt.srcElement : null);
if ((evt.keyCode == 13) && (node.type=="text"))
{return false;}
}

document.onkeypress = stopRKey;
</script>
<?php
if($_POST['saved'] == 1) {
if($_POST['filepath'] != "files/") {
echo "<font color='red'>SETTINGS SAVED TO FILE</font><br/><br/>"; }
else {
echo "<font color='red'>NO FILE SELECTED YET.. PLEASE DO SO </font><a href='SaveSettings.php'>HERE</a><br/><br/>"; }
}
?>

<form method='post' action='UserHooks.php'>

<h2>User Hooks</h2>

Sometimes it may be convenient to step in during the generation
process: to modify the built-in cross sections, to veto undesirable
events or simply to collect statistics at various stages of the
evolution. There is a base class <code>UserHooks</code> that gives 
you this access at a few selected places. This class in itself does 
nothing; the idea is that you should write your own derived class
for your task. A few very simple derived classes come with the 
program, mainly as illustration. 

<p/>
For a derived class to be called during the execution, a pointer to
an object of this class should be handed in with the 
<br/>
<?php $filepath = $_GET["filepath"];
echo "<a href='ProgramFlow.php?filepath=".$filepath."' target='page'>";?>
<code>pythia.setUserHooksPtr( UserHooks*)</code></a> method.

<p/>
There are four distinct sets of routines. Ordered by increasing 
complexity, rather than by their appearance in the event-generation
sequence, they are: 
<br/>(i) Ones that gives you access to the event record in between
the process-level and parton-level steps, or in between the 
parton-level and hadron-level ones. You can study the event record
and decide whether to veto this event.
<br/>(ii) Ones that allow you to set a scale at with the combined
parton-level MI+ISR+FSR downwards evolution in <i>pT</i> is
temporarily interrupted, so the event can be studied and either 
vetoed or allowed to continue the evolution.
<br/>(iii) Ones that allow you to to study the event after the first
few ISR/FSR emissions, so the event can be vetoed or allowed to 
continue the evolution.
<br/>(iv) Ones that gives you access to the properties of the trial 
hard process, so that you can modify the internal Pythia cross section
by your own correction factors.
<br/>They are described further in the following. 

<h3>Interrupt between the main generation levels</h3>

If your derived class redefines the 
<br/><code>bool canVetoProcessLevel()</code>
<br/>method to return <code>true</code>, then the method 
<br/><code>bool doVetoProcessLevel(const Event& process)</code>
<br/>will be called immediately after a hard process has been selected and
stored in the <code>process</code> event record. You can study, but not
modify, this record. Based on that you can decide whether to veto the
event or let it continue to evolve. If you veto, then this event is not
counted among the accepted ones, and do not contribute to the estimated
cross section. The <code>pytha.next()</code> method will begin a 
completely new event, so the vetoed event will not appear in the 
output of <code>pythia.next()</code>.

<p/>
Note that this is different from setting the flag
<?php $filepath = $_GET["filepath"];
echo "<a href='MasterSwitches.php?filepath=".$filepath."' target='page'>";?><code>PartonLevel:all = off</code></a>. 
Also in this case the event
generation will stop after the process level, but an event generated
up to this point is considered perfectly acceptable, and cross sections
are not affected. That is, this option is intended for simple studies of 
hard processes, where one can save time by not generating the rest of the
story. By contrast, the <code>doVetoProcessLevel()</code> allows you to
throw away uninteresting events at an early stage to save time that way, 
but those events that do survive the veto are allowed to develop into  
complete final states (unless flags have been set otherwise). 

<p/>
The 
<br/><code>bool canVetoPartonLevel()</code>
<br/>and 
<br/><code>bool doVetoPartonLevel(const Event& event)</code>
<br/>are exact analogues to the above two methods, except that these
ones are called after the parton level, i.e. when showers, multiple
interactions and beam remnants have been set up, but hadronization and
decays have not yet been performed. Information is now made available in 
the <code>event</code> event record. The difference relative to the 
<?php $filepath = $_GET["filepath"];
echo "<a href='MasterSwitches.php?filepath=".$filepath."' target='page'>";?><code>HadronLevel:all = off</code></a> 
flag setting follows the same pattern as above.

<p/> 
The effect of the vetoes can be studied in the output of the 
<code>pythia.statistics()</code> method. The "Selected" column represents
the number of events that were found acceptable by the internal Pythia
machinery, whereas the "Accepted" one are the events that also survived 
the user cuts. 

<h3>Interrupt during the parton-level evolution, at a <i>pT</i> scale</h3>

During the parton-level evolution, multiple interactions (MI), 
initial-state radiation (ISR) and final-state radiation (FSR)
are normally evolved downwards in
one interleaved evolution sequence of decreasing <i>pT</i> values.
For some applications, e.g  matrix-element-matching approaches, it
may be convenient to stop the evolution temporarily when the "hard"
emissions have been considered, but before continuing with the more
time-consuming soft activity. Based on these hard partons one can make
a decision whether the event at all falls in the intended event class,
e.g. has the "right" number of parton-level jets. If yes then, as for 
the methods above, the evolution will continue all the way up to a 
complete event. Also as above, if no, then the event will not be 
considered in the final cross section. 

<p/>
In this subsection we outline the possibility to interrupt at a given 
<i>pT</i> scale, in the next to interrupt after a given number of 
emissions.

<p/>
To use this possibility you need to redefine 
<br/><code>bool canVetoPT()</code>
<br/>to return <code>true</code> and
<br/><code>double scaleVetoPT()</code>
<br/>to return the <i>pT</i> scale at which you want to study the event.
 
<p/>
The key routine, where you decide whether the event should be vetoed
(return <code>true</code>) or not (<code>false</code>), is  
<br/><code>bool doVetoPT(int iPos, const Event& event)</code>
<br/>Here
<br/><code>iPos</code> is the position/status when the routine is
called: 
<br/>= 0 when no MI, ISR or FSR occured above the veto scale;
<br/>= 1 when inside the interleaved MI + ISR + FSR evolution, 
after an MI process;  
<br/>= 2 when inside the interleaved MI + ISR + FSR evolution, 
after an ISR emission;
<br/>= 3 when inside the interleaved MI + ISR + FSR evolution, 
after an FSR emission;
<br/>= 4 for the optional case where FSR is deferred from the interleaved
evolution and only considered separately afterward (then alternative 3 
would never occur);
<br/>= 5 is for subsequent resonance decays, and is called once
for each decay in a chain such as <i>t -> b W, W -> u dbar</i>.

<p/>
The event record contains a list of all partons generated so far, also 
including intermediate ones not part of the "current final state",
and also those from further multiple interactions. This may not be
desirable for comparisons with matrix-element calculations. The method 
<br/><code>void subEvent(const Event& event, bool isHardest = true)</code>
<br/> offers a simple recipe to extract a list of only the current 
partons from the hardest interaction, as relevant for <code>iPos</code>
codes 0 - 4. With <code>isHardest = false</code> instead the latest 
"subprocess" is extracted, as relevant when <code>iPos</code> is 5,
where it corresponds to the partons in the currently considered decay.

<p/>
The result is stored in the class member <code>Event workEvent</code>.
The <code>daughter1()</code> and <code>daughter2()</code> both return
the position in the original event record (<code>process</code> or
<code>event</code>), so you can trace the full history, if of interest. 
The <code>workEvent</code> can e.g. be sent on to a 
<?php $filepath = $_GET["filepath"];
echo "<a href='EventAnalysis.php?filepath=".$filepath."' target='page'>";?>jet clustering algorithm</a>.
You are free to edit  <code>workEvent</code> as you desire, e.g. boost 
to its rest frame before analysis, or remove particles that should 
not be analyzed.

<h3>Interrupt during the parton-level evolution, after a step</h3>

This option is closely related to the one above, so we do not repeat
the introduction, nor the possibilities to study the event record,
also by using <code>subEvent(...)</code>.  
What is different is that this method gives access to the event as
it looks like after each of the first few steps in the downwards
evolution, irrespectively of the <i>pT</i> scales of these branchings.
Furthermore, it is here assumed that the focus is on the hardest
subprocess, so that ISR/FSR emissions associated with additional MI's
are not considered.

<p/>
To use the possibility to study the event after the first steps you need 
to redefine 
<br/><code>bool canVetoStep()</code>
<br/>to return <code>true</code> and
<br/><code>int numberVetoStep()</code>
<br/>to return up to how many steps each of ISR and FSR (for the hardest
interaction) that you want to be able to study. The number of steps 
defaults to the first one only. 

<p/>
The key routine, where you decide whether the event should be vetoed
(return <code>true</code>) or not (<code>false</code>), is  
<br/><code>bool doVetoStep( int iPos, int nISR, int nFSR, 
const Event& event)</code>
<br/>Here 
<br/><code>iPos</code> is the position from where the routine has been
called, options 2 - 5 of the <code>doVetoPT(...)</code> routine
above, while options 0 and 1 are not relevant here;
<br/><code>nISR</code> is the number of ISR emissions in the hardest 
process so far; and 
<br/><code>nFSR</code> is the number of FSR emissions in the hardest 
process so far. 
<br/>For resonance decays, <code>iPos = 5</code>, the <code>nISR</code>
is set 0 and <code>nFSR</code> refers to the number of emissions in
the currently studied system. 

<h3>Modify cross-sections</h3>

If you want to modify a cross section you need to redefine 
<br/><code>bool canModifySigma()</code> 
<br/>to return <code>true</code> and
<br/><code>double multiplySigmaBy(const SigmaProcess* sigmaProcessPtr,
const PhaseSpace* phaseSpacePtr, bool inEvent)</code> 
<br/>to provide the factor by
which you want to see the cross section modified. If you return unity
then the normal cross section is obtained. Note that, unlike the 
methods above, these modifications do not lead to a difference between
the number of "selected" events and the number of "accepted" ones, 
since the modifications occur already before the "selected" level.
The integrated cross section of a process is modified, of course.

<p/>
What makes the <code>multiplySigmaBy(...)</code> routine somewhat 
tricky to write is that the hard-process event has not yet been
constructed, so one is restricted to use the information available
in the phase-space and cross-section objects currently being accessed.
Which of their  methods are applicable depends on the process,
in particular the number of final-state particles. The 
<code>UserHooks</code> code contains explicit instructions about
which methods provide meaningful information. 

<p/>
The <code>inEvent</code> flag is <code>true</code> when this method is 
called from within the event-generation machinery and <code>false</code>
when it is called at the initialization stage of the run, when the 
cross section is explored to find a maximum for later Monte Carlo usage. 
Cross-section modifications should be independent of this flag,
for consistency, but if <code> multiplySigmaBy(...)</code> is used to
collect statistics on the original kinematics distributions before cuts,
then it is important to be able to exclude the initialization stage
from comparisons.

<p/>
Note that the cross section is only modifiable for normal hard processes.
It does not affect the cross section in further multiple interactions,
nor in elastic/diffractive/minimum-bias events.

<p/>
One derived class is supplied as an example how this facility can be used
to reweight cross sections in the same spirit as is done with QCD cross
sections for the minimum-bias/underlying-event description:
<p/><code>class&nbsp; </code><strong> SuppressSmallPT( pT0timesMI, numberAlphaS, useSameAlphaSasMI) &nbsp;</strong> <br/>
suppress small-<i>pT</i> production for <i>2 -> 2</i> processes
only, while leaving other processes unaffected. The basic suppression
factor is <i>pT^4 / ((k*pT0)^2 + pT^2)^2</i>, where <i>pT</i>
refers to the current hard subprocess and <i>pT0</i> is the same
energy-dependent dampening scale as used for 
<?php $filepath = $_GET["filepath"];
echo "<a href='MultipleInteractions.php?filepath=".$filepath."' target='page'>";?>multiple interactions</a>. 
The optional arguments provide further variability. 
<br/><code>argument</code><strong> pT0timesMI </strong>  :  
corresponds to the additional factor <i>k</i> in the above formula. 
It is by default equal to 1 but can be used to explore deviations from 
the expected value.
  
<br/><code>argument</code><strong> numberAlphaS </strong>  :   
if this number <i>n</i> is bigger than the default 0, the 
corresponding number of <i>alpha_strong</i> factors is also 
reweighted from the normal renormalization scale to a modified one,
i.e. a further suppression factor
<i>( alpha_s((k*pT0)^2 + Q^2_ren) / alpha_s(Q^2_ren) )^n</i>
is introduced.
  
<br/><code>argument</code><strong> useSameAlphaSasMI </strong>  :  
regulates which kind of new <i>alpha_strong</i> value is evaluated
for the numerator in the above expression. It is by default the same 
as set for multiple interactions (i.e. same starting value at 
<i>M_Z</i> and same order of running), but if <code>false</code> 
instead the one for hard subprocesses. The denominator 
<i>alpha_s(Q^2_ren)</i> is always the value used for the "original", 
unweighted cross section. 
  
  
 
<h3>Final comments</h3>

All the possibilities above can be combined freely and also be combined
with the standard flags. An event would then survive only if it survived
each of the possible veto methods. There are no hidden interdependencies 
in this game, but of course some combinations may not be particularly 
meaningful. For instance, if you set <code>PartonLevel:all = off</code> 
then the <code>doVetoPT(...)</code> and <code>doVetoPartonLevel(...)</code> 
locations in the code are not even reached, so they would never be called. 

<p/>
An example how the above methods can be used for toy studies is found in 
<code>main10.cc</code>.

</body>
</html>

<!-- Copyright (C) 2007 Torbjorn Sjostrand -->