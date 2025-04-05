import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';
import 'package:flutter_svg/flutter_svg.dart';
import 'package:shimmer/shimmer.dart';
import 'package:intl/intl.dart';
import 'package:speech_to_text/speech_to_text.dart' as stt;
import 'package:avatar_glow/avatar_glow.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'AC Remote Pro',
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
        primarySwatch: Colors.blue,
        scaffoldBackgroundColor: const Color(0xFFF5F7FA),
      ),
      home: const ACRemoteScreen(),
    );
  }
}

class ACRemoteScreen extends StatefulWidget {
  const ACRemoteScreen({super.key});

  @override
  State<ACRemoteScreen> createState() => _ACRemoteScreenState();
}

class _ACRemoteScreenState extends State<ACRemoteScreen> {
  String _ipAddress = "192.168.212.129";
  late stt.SpeechToText _speech;
  bool _isListening = false;
  String _lastWords = '';
  String _currentMode = "auto";
  double _currentTemp = 0.0;
  String _acSetting = "OFF";
  String _motionStatus = "no_motion";
  bool _isLoading = false;
  bool _showIpDialog = false;
  final TextEditingController _ipController = TextEditingController();

  @override
  void initState() {
    super.initState();
    _ipController.text = _ipAddress;
    _speech = stt.SpeechToText();
    _initializeSpeech();
    _fetchStatus();
    _setupPeriodicUpdates();
  }

  void _initializeSpeech() async {
    bool available = await _speech.initialize(
      onStatus: (status) => print('Speech status: $status'),
      onError: (error) => print('Speech error: $error'),
    );
    if (available) {
      print("Speech recognition initialized");
    } else {
      print("Speech recognition not available");
    }
  }

  void _setupPeriodicUpdates() {
    Future.delayed(const Duration(seconds: 2), () {
      _fetchStatus();
      if (mounted) _setupPeriodicUpdates();
    });
  }

  Future<void> _fetchStatus() async {
    try {
      final response = await http.get(
        Uri.parse('http://$_ipAddress/api/status'),
        headers: {'Accept': 'application/json'},
      ).timeout(const Duration(seconds: 3));

      if (response.statusCode == 200) {
        final data = json.decode(response.body);
        setState(() {
          _currentMode = data['mode'];
          _currentTemp = data['temperature']?.toDouble() ?? 0.0;
          _acSetting = data['ac_setting'] ?? "OFF";
          _motionStatus = data['motion'] ?? "no_motion";
        });
      }
    } catch (e) {
      debugPrint("Error fetching status: $e");
    }
  }

  Future<void> _sendCommand(String command) async {
    try {
      setState(() => _isLoading = true);
      final response = await http.get(
        Uri.parse('http://$_ipAddress/$command'),
        headers: {'Accept': 'application/json'},
      ).timeout(const Duration(seconds: 3));

      if (response.statusCode == 200) {
        await _fetchStatus();
      }
    } catch (e) {
      _showErrorSnackbar('Failed to send command');
    } finally {
      if (mounted) setState(() => _isLoading = false);
    }
  }

  Future<void> _setTemperature(String temp) async {
    try {
      setState(() => _isLoading = true);
      final response = await http.get(
        Uri.parse('http://$_ipAddress/settemp?temp=$temp'),
        headers: {'Accept': 'application/json'},
      ).timeout(const Duration(seconds: 3));

      if (response.statusCode == 200) {
        setState(() => _acSetting = temp);
      }
    } catch (e) {
      _showErrorSnackbar('Failed to set temperature');
    } finally {
      if (mounted) setState(() => _isLoading = false);
    }
  }

  Future<void> _sendVoiceCommand(String command) async {
    try {
      setState(() => _isLoading = true);
      final response = await http.get(
        Uri.parse('http://$_ipAddress/voice?command=$command'),
        headers: {'Accept': 'application/json'},
      ).timeout(const Duration(seconds: 3));

      if (response.statusCode == 200) {
        await _fetchStatus();
      }
    } catch (e) {
      _showErrorSnackbar('Failed to process voice command');
    } finally {
      if (mounted) setState(() => _isLoading = false);
    }
  }

  void _listen() async {
    if (!_isListening) {
      bool available = await _speech.initialize(
        onStatus: (status) => print('onStatus: $status'),
        onError: (error) => print('onError: $error'),
      );
      if (available) {
        setState(() => _isListening = true);
        _speech.listen(
          onResult: (result) {
            setState(() {
              _lastWords = result.recognizedWords;
              if (result.finalResult) {
                _processVoiceCommand(_lastWords);
                _lastWords = '';
              }
            });
          },
        );
      }
    } else {
      setState(() => _isListening = false);
      _speech.stop();
    }
  }

  void _processVoiceCommand(String command) {
    command = command.toLowerCase();
    print("Processing voice command: $command");

    if (command.contains('auto') || command.contains('automatic')) {
      _sendCommand("auto");
    } 
    else if (command.contains('manual')) {
      _sendCommand("manual");
    }
    else if (command.contains('turn on') || command.contains('switch on') || command.contains('on')) {
      if (_acSetting == "OFF") {
        _setTemperature("24C");
      }
    }
    else if (command.contains('turn off') || command.contains('switch off') || command.contains('off')) {
      _setTemperature("OFF");
    }
    else if (command.contains('increase') || command.contains('up') || command.contains('hotter')) {
      _adjustTemperature(1);
    }
    else if (command.contains('decrease') || command.contains('down') || command.contains('cooler')) {
      _adjustTemperature(-1);
    }
    else {
      // Try to extract temperature number
      final regex = RegExp(r'\b(1[7-9]|2[0-9]|30)\b');
      final match = regex.firstMatch(command);
      if (match != null) {
        _setTemperature("${match.group(0)}C");
      } else {
        _showErrorSnackbar("Command not recognized");
      }
    }
  }

  void _showErrorSnackbar(String message) {
    ScaffoldMessenger.of(context).showSnackBar(
      SnackBar(
        content: Text(message),
        behavior: SnackBarBehavior.floating,
        shape: RoundedRectangleBorder(
          borderRadius: BorderRadius.circular(10)),
        margin: const EdgeInsets.all(10),
      ),
    );
  }

  void _showIpConfigDialog() {
    showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: const Text('Configure Server'),
        content: TextField(
          controller: _ipController,
          decoration: const InputDecoration(
            labelText: 'IP Address',
            hintText: '192.168.x.x',
            border: OutlineInputBorder(),
          ),
          keyboardType: TextInputType.numberWithOptions(decimal: true),
        ),
        actions: [
          TextButton(
            onPressed: () => Navigator.pop(context),
            child: const Text('Cancel'),
          ),
          ElevatedButton(
            onPressed: () {
              setState(() => _ipAddress = _ipController.text);
              Navigator.pop(context);
              _fetchStatus();
            },
            child: const Text('Save'),
          ),
        ],
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('AC Remote Pro'),
        actions: [
          IconButton(
            icon: const Icon(Icons.settings),
            onPressed: _showIpConfigDialog,
          ),
        ],
      ),
      body: _isLoading
          ? _buildLoadingState()
          : SingleChildScrollView(
              padding: const EdgeInsets.all(20),
              child: Column(
                children: [
                  _buildStatusCard(),
                  const SizedBox(height: 24),
                  _buildModeSelector(),
                  if (_currentMode == "manual") ...[
                    const SizedBox(height: 24),
                    _buildTemperatureControl(),
                    const SizedBox(height: 16),
                    _buildPowerControls(),
                  ],
                ],
              ),
            ),
      floatingActionButton: _buildVoiceButton(),
      floatingActionButtonLocation: FloatingActionButtonLocation.centerFloat,
    );
  }

  Widget _buildVoiceButton() {
    return Column(
      mainAxisAlignment: MainAxisAlignment.end,
      children: [
        AvatarGlow(
          animate: _isListening,
          glowColor: Colors.blue,
          endRadius: 60.0,
          duration: const Duration(milliseconds: 2000),
          repeat: true,
          showTwoGlows: true,
          repeatPauseDuration: const Duration(milliseconds: 100),
          child: FloatingActionButton(
            onPressed: _listen,
            child: Icon(_isListening ? Icons.mic : Icons.mic_none),
          ),
        ),
        const SizedBox(height: 10),
        Text(
          _isListening ? 'Listening...' : 'Tap to speak',
          style: const TextStyle(fontSize: 16),
        ),
        if (_lastWords.isNotEmpty)
          Padding(
            padding: const EdgeInsets.all(8.0),
            child: Text(
              _lastWords,
              style: const TextStyle(fontSize: 14, color: Colors.grey),
            ),
          ),
      ],
    );
  }

  Widget _buildLoadingState() {
    return Center(
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          const CircularProgressIndicator(),
          const SizedBox(height: 20),
          Text(
            'Connecting to AC Controller...',
            style: Theme.of(context).textTheme.titleMedium,
          ),
        ],
      ),
    );
  }

  Widget _buildStatusCard() {
    return Card(
      elevation: 2,
      shape: RoundedRectangleBorder(
        borderRadius: BorderRadius.circular(12)),
      child: Padding(
        padding: const EdgeInsets.all(16),
        child: Column(
          children: [
            Row(
              mainAxisAlignment: MainAxisAlignment.spaceBetween,
              children: [
                const Text('Current Mode:', style: TextStyle(fontWeight: FontWeight.bold)),
                Chip(
                  label: Text(_currentMode.toUpperCase()),
                  backgroundColor: _currentMode == "auto" ? Colors.green[100] : Colors.orange[100],
                ),
              ],
            ),
            const Divider(height: 24),
            Row(
              mainAxisAlignment: MainAxisAlignment.spaceBetween,
              children: [
                const Text('Room Temperature:', style: TextStyle(fontWeight: FontWeight.bold)),
                Text(
                  '${_currentTemp.toStringAsFixed(1)}°C',
                  style: const TextStyle(fontSize: 16, fontWeight: FontWeight.bold),
                ),
              ],
            ),
            const Divider(height: 24),
            Row(
              mainAxisAlignment: MainAxisAlignment.spaceBetween,
              children: [
                const Text('AC Setting:', style: TextStyle(fontWeight: FontWeight.bold)),
                Text(
                  _acSetting == "OFF" ? "OFF" : "${_acSetting.substring(0, _acSetting.length - 1)}°C",
                  style: TextStyle(
                    fontSize: 16,
                    fontWeight: FontWeight.bold,
                    color: _acSetting == "OFF" ? Colors.red : Colors.green),
                ),
              ],
            ),
            const Divider(height: 24),
            Row(
              mainAxisAlignment: MainAxisAlignment.spaceBetween,
              children: [
                const Text('Motion Status:', style: TextStyle(fontWeight: FontWeight.bold)),
                Row(
                  children: [
                    Icon(
                      _motionStatus == "detected" ? Icons.directions_run : Icons.timer,
                      color: _motionStatus == "detected" ? Colors.green : Colors.grey,
                    ),
                    const SizedBox(width: 8),
                    Text(_motionStatus == "detected" ? "DETECTED" : "NO MOTION"),
                  ],
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildModeSelector() {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        const Text('Operation Mode', style: TextStyle(fontSize: 16, fontWeight: FontWeight.bold)),
        const SizedBox(height: 8),
        SegmentedButton(
          segments: const [
            ButtonSegment(
              value: "auto",
              label: Text('AUTO'),
              icon: Icon(Icons.auto_awesome)),
            ButtonSegment(
              value: "manual",
              label: Text('MANUAL'),
              icon: Icon(Icons.touch_app)),
          ],
          selected: <String>{_currentMode},
          onSelectionChanged: (Set<String> newSelection) {
            _sendCommand(newSelection.first);
          },
        ),
      ],
    );
  }

  Widget _buildTemperatureControl() {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        const Text('Temperature Control', style: TextStyle(fontSize: 16, fontWeight: FontWeight.bold)),
        const SizedBox(height: 12),
        Container(
          decoration: BoxDecoration(
            color: Colors.white,
            borderRadius: BorderRadius.circular(12),
            boxShadow: [
              BoxShadow(
                color: Colors.grey.withOpacity(0.1),
                spreadRadius: 2,
                blurRadius: 8,
                offset: const Offset(0, 4)),
            ],
          ),
          padding: const EdgeInsets.all(16),
          child: Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              IconButton(
                icon: const Icon(Icons.remove_circle_outline, size: 32),
                color: Colors.blue,
                onPressed: () => _adjustTemperature(-1),
              ),
              Text(
                _acSetting == "OFF" ? "OFF" : _acSetting.substring(0, _acSetting.length - 1),
                style: const TextStyle(fontSize: 32, fontWeight: FontWeight.bold),
              ),
              IconButton(
                icon: const Icon(Icons.add_circle_outline, size: 32),
                color: Colors.blue,
                onPressed: () => _adjustTemperature(1),
              ),
            ],
          ),
        ),
      ],
    );
  }

  Widget _buildPowerControls() {
    return Row(
      children: [
        Expanded(
          child: OutlinedButton.icon(
            icon: const Icon(Icons.refresh),
            label: const Text('RESEND'),
            style: OutlinedButton.styleFrom(
              padding: const EdgeInsets.symmetric(vertical: 16),
              side: const BorderSide(color: Colors.blue)),
            onPressed: () => _sendCommand("resend"),
          ),
        ),
        const SizedBox(width: 16),
        Expanded(
          child: ElevatedButton.icon(
            icon: const Icon(Icons.power_settings_new),
            label: const Text('POWER OFF'),
            style: ElevatedButton.styleFrom(
              backgroundColor: Colors.red,
              padding: const EdgeInsets.symmetric(vertical: 16)),
            onPressed: () => _setTemperature("OFF"),
          ),
        ),
      ],
    );
  }

  Future<void> _adjustTemperature(int change) async {
    if (_acSetting == "OFF" && change > 0) {
      await _setTemperature("24C");
      return;
    }
    
    if (_acSetting.endsWith("C")) {
      int currentTemp = int.parse(_acSetting.substring(0, _acSetting.length - 1));
      int newTemp = currentTemp + change;
      
      if (newTemp >= 17 && newTemp <= 30) {
        await _setTemperature("${newTemp}C");
      }
    }
  }
}